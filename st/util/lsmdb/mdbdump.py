#!/usr/bin/env python3
"""
MiniDB .db file inspector.

Dump (default):        mdbdump.py [-b] <path>
CSV receipts:          mdbdump.py --csv-receipts <path>
CSV statistics:        mdbdump.py --csv-stats <path>
Concise:               mdbdump.py --csv <path>

Struct offsets match BCC 3.1 16-bit DOS layout.
"""
import struct, sys, os, csv, io

PAGE_SIZE=512; HDR_SIZE=16
PAGE_FREE,PAGE_DBINFO,PAGE_BTREE_I,PAGE_BTREE_L,PAGE_DATA,PAGE_STATS=0,1,2,3,4,5
PNAME={0:"FREE",1:"DBINFO",2:"BTREE_I",3:"BTREE_L",4:"DATA",5:"STATS"}
RECEIPT_MAGIC=0x6719

# Receipt offsets (BCC 3.1, int=2, UCHAR=1, enum=2, long=4, double=8)
# ofs:0 Magic(W) 2 Number(l) 6 Tag(W) 8 nStat(W) 10 bExt(B)
# 11 Date(h) 13 Time(h) 15 Booth(h) 17 City(21) 38 Phone(17)
# 55 Amount(h) 57 Elapsed(l) 61 VPM(d) 69 Ceil(d) 77 Percent(h)
# 79 Value(d) 87 Tax(d) 95 Tax2(d) 103 DDummy(d)

def parse_receipt(sd):
    """Parse a 111-byte receipt. Returns dict or None."""
    if len(sd) < 111: return None
    mn = struct.unpack_from('<H', sd, 0)[0]
    if mn != RECEIPT_MAGIC: return None
    return {
        'number':  struct.unpack_from('<l', sd, 2)[0],
        'tag':     struct.unpack_from('<H', sd, 6)[0],
        'nstat':   struct.unpack_from('<H', sd, 8)[0],
        'date':    struct.unpack_from('<h', sd, 11)[0],
        'time':    struct.unpack_from('<h', sd, 13)[0],
        'booth':   struct.unpack_from('<h', sd, 15)[0],
        'city':    sd[17:38].split(b'\0')[0].decode('latin-1',errors='replace'),
        'phone':   sd[38:55].split(b'\0')[0].decode('latin-1',errors='replace'),
        'amount':  struct.unpack_from('<h', sd, 55)[0],
        'elapsed': struct.unpack_from('<l', sd, 57)[0],
        'vpm':     struct.unpack_from('<d', sd, 61)[0],
        'ceil':    struct.unpack_from('<d', sd, 69)[0],
        'percent': struct.unpack_from('<h', sd, 77)[0],
        'value':   struct.unpack_from('<d', sd, 79)[0],
        'tax':     struct.unpack_from('<d', sd, 87)[0],
        'tax2':    struct.unpack_from('<d', sd, 95)[0],
    }

# ---------------------------------------------------------------------------
def ph(page): return struct.unpack_from('<HHHHl',page,0)

# Dump formatters (human-readable)
# ---------------------------------------------------------------------------
def dump_dbinfo(page, out):
    m,pt,nk,fo,r=ph(page)
    if m!=0xDBDB or pt!=PAGE_DBINFO: out.write('  *** INVALID ***\n'); return
    ver,root,seq,nrecs,narc,sa,fh=struct.unpack_from('<H6l',page,HDR_SIZE)
    out.write(f'  Version={ver}  RootPage={root}  Sequence={seq}\n')
    out.write(f'  NumReceipts={nrecs}  NumArchived={narc}  StatsAnchor={sa}  FreeHead={fh}\n')

def dump_leaf(page, out):
    m,pt,nk,fo,r=ph(page)
    if nk==0: return
    out.write(f'  NumKeys={nk}  RightSibling={r}\n')
    for i in range(nk):
        off=HDR_SIZE+i*12
        num,booth,sp,fl=struct.unpack_from('<lhlH',page,off)
        pn=sp>>8; sl=sp&3; d='(del)' if fl&1 else ''
        out.write(f'  [{i}] num={num} booth={booth} page={pn} slot={sl} {d}\n')

def dump_data(page, out):
    m,pt,nk,fo,r=ph(page)
    if nk==0: return
    out.write(f'  NumKeys={nk}\n')
    for sl in range(nk):
        off=HDR_SIZE+sl*111
        r=parse_receipt(page[off:off+111])
        if not r: out.write(f'  [slot {sl}] BAD magic\n'); continue
        dl='(DEL)' if r['nstat']&0x0020 else ''
        out.write(f'  [slot {sl}] #{r["number"]} booth={r["booth"]} date={r["date"]} t={r["time"]} {dl}\n')
        out.write(f'           city={r["city"]} phone={r["phone"]} amount={r["amount"]} elapsed={r["elapsed"]}s\n')
        out.write(f'           val={r["value"]:.2f} tax={r["tax"]:.2f} vpm={r["vpm"]:.4f} ceil={r["ceil"]} pct={r["percent"]}\n')

def dump_stats(page, out):
    m,pt,nk,fo,r=ph(page)
    out.write(f'  NumKeys={nk}\n')
    periods=['YEAR','MONTH','WEEK','DAY','TURN']
    svcs=['TEL','SPECIAL_TEL','FAX','TELEX','CARD','OTHER']
    pay=page[HDR_SIZE:]
    for p in range(5):
        out.write(f'  --- {periods[p]} ---\n')
        for s in range(6):
            off=(p*6+s)*30
            if off+30>len(pay): break
            rec,tmin,pmin,v,tx=struct.unpack_from('<hdddd',pay,off)
            if rec or v:
                out.write(f'    {svcs[s]:12} rec={rec} talk={tmin:.2f} paid={pmin:.2f} val={v:.2f} tax={tx:.2f}\n')

# ---------------------------------------------------------------------------
def dump_stats(page, out):
    """Read a DS_ENTRY from a PAGE_STATS page and print its contents."""
    m,pt,nk,fo,r=ph(page)
    if pt != PAGE_STATS: return
    # DS_ENTRY starts at MINIDB_HDR_SIZE
    off = MINIDB_HDR_SIZE
    # Read From/To RANGETAG
    FromTime, FromDate, FromNum = struct.unpack_from('<HHl', page, off)
    ToTime, ToDate, ToNum = struct.unpack_from('<HHl', page, off+8)
    out.write(f'  From: t={FromTime} d={FromDate} num={FromNum}\n')
    out.write(f'  To:   t={ToTime} d={ToDate} num={ToNum}\n')
    svcs = ['Tel.Nal','Tel.Inter','SpTel.Nal','SpTel.Inter','Fax.Nal','Fax.Inter',
            'Net.Nal','Net.Inter','Cards','Other']
    # Each ITEM: receipts(WORD) talkMin(double) paidMin(double) value(double) tax(double) = 34 bytes
    # Offsets within DS_ENTRY need to match BCC 3.1 layout
    # From:0 To:8 Tel.Nal:16 Tel.Inter:50 SpTel.Nal:84 SpTel.Inter:118 Fax.Nal:152 Fax.Inter:186
    # Net.Nal:220 Net.Inter:254 Cards:288 Other:322
    item_offsets = [16, 50, 84, 118, 152, 186, 220, 254, 288, 322]
    for svc, ioff in zip(svcs, item_offsets):
        if ioff + 34 > MINIDB_PAYLOAD: break
        rec = struct.unpack_from('<H', page, off+ioff)[0]
        tmin, pmin, val, tx = struct.unpack_from('<dddd', page, off+ioff+2)
        if rec or val:
            out.write(f'    {svc:12} rec={rec} talk={tmin:.2f} paid={pmin:.2f} val={val:.2f} tax={tx:.2f}\n')
    # DialErrors/ComErrors at offsets determined by struct layout
    # ... (skip for brevity)
    """Write stats page as CSV rows."""
    m,pt,nk,fo,r=ph(page)
    periods=['YEAR','MONTH','WEEK','DAY','TURN']
    svcs=['TEL','SPECIAL_TEL','FAX','TELEX','CARD','OTHER']
    pay=page[HDR_SIZE:]
    for p in range(5):
        for s in range(6):
            off=(p*6+s)*30
            if off+30>len(pay): break
            rec,tmin,pmin,v,tx=struct.unpack_from('<hdddd',pay,off)
            if rec or v:
                writer.writerow([periods[p], svcs[s], rec, f'{tmin:.2f}', f'{pmin:.2f}', f'{v:.2f}', f'{tx:.2f}'])

# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    import argparse
    parser = argparse.ArgumentParser(
        description='Dump MiniDB .db file contents.',
        epilog='Struct offsets match BCC 3.1 16-bit DOS. Default: dump all pages.')
    parser.add_argument('path', nargs='?', default='RX.db',
                        help='Path to .db file (default: RX.db)')
    parser.add_argument('-b', '--brief', action='store_true',
                        help='Skip free pages (dump mode only)')
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--csv', action='store_true',
                       help='Concise CSV: both receipts and stats')
    group.add_argument('--csv-receipts', action='store_true',
                       help='Receipts as CSV')
    group.add_argument('--csv-stats', action='store_true',
                       help='Statistics as CSV')
    args = parser.parse_args()

    path = args.path
    if not os.path.exists(path):
        print(f'ERROR: {path} not found', file=sys.stderr)
        sys.exit(1)

    with open(path, 'rb') as f:
        data = f.read()
    np = len(data) // PAGE_SIZE

    # Collect receipts and stats from all pages
    receipts = []
    stats = []
    for pn in range(np):
        page = data[pn*PAGE_SIZE:(pn+1)*PAGE_SIZE]
        m,pt,nk,fo,r = ph(page)
        if pt == PAGE_DATA:
            for sl in range(nk):
                off = HDR_SIZE+sl*111
                rcp = parse_receipt(page[off:off+111])
                if rcp: receipts.append(rcp)
        elif pt == PAGE_STATS:
            periods = ['YEAR','MONTH','WEEK','DAY','TURN']
            svcs = ['TEL','SPECIAL_TEL','FAX','TELEX','CARD','OTHER']
            pay = page[HDR_SIZE:]
            for p in range(5):
                for s in range(6):
                    off = (p*6+s)*30
                    if off+30 > len(pay): break
                    rec,tmin,pmin,v,tx = struct.unpack_from('<hdddd',pay,off)
                    if rec or v:
                        stats.append((periods[p], svcs[s], rec, tmin, pmin, v, tx))

    # CSV output
    if args.csv or args.csv_receipts or args.csv_stats:
        w = csv.writer(sys.stdout)
        if args.csv_receipts or args.csv:
            w.writerow(['number','booth','date','time','city','phone',
                        'amount','elapsed_s','value','tax','vpm','ceil_min','percent','deleted'])
            for r in receipts:
                w.writerow([r['number'], r['booth'], r['date'], r['time'],
                           r['city'], r['phone'], r['amount'], r['elapsed'],
                           f'{r["value"]:.2f}', f'{r["tax"]:.2f}',
                           f'{r["vpm"]:.4f}', r['ceil'], r['percent'],
                           '(deleted)' if r['nstat']&0x0020 else ''])
            if not args.csv and not args.csv_stats:
                return
        if args.csv_stats or args.csv:
            if args.csv: w.writerow([])  # blank separator
            w.writerow(['period','service','receipts','talk_min','paid_min','value','tax'])
            for period, svc, rec, tmin, pmin, v, tx in stats:
                w.writerow([period, svc, rec, f'{tmin:.2f}', f'{pmin:.2f}', f'{v:.2f}', f'{tx:.2f}'])
        return

    # Human-readable dump
    if args.brief:
        print(f'File: {path}  ({len(data)} bytes, {np} pages)')
    for pn in range(np):
        page = data[pn*PAGE_SIZE:(pn+1)*PAGE_SIZE]
        m,pt,nk,fo,r = ph(page)
        pn2 = PNAME.get(pt, f'TYPE{pt}')
        if args.brief and pt == PAGE_FREE:
            continue
        print(f'\n{"="*55}\nPage {pn} [{pn2}] @0x{pn*PAGE_SIZE:04x}\n{"="*55}')
        if pt == PAGE_DBINFO:
            dump_dbinfo(page, sys.stdout)
        elif pt == PAGE_BTREE_L:
            dump_leaf(page, sys.stdout)
        elif pt == PAGE_DATA:
            dump_data(page, sys.stdout)
        elif pt == PAGE_STATS:
            dump_stats(page, sys.stdout)
        elif pt == PAGE_FREE:
            print(f'  (free sibling={r})')
        else:
            print(f'  (type {pt})')

    dp = sum(1 for p in range(np) if ph(data[p*PAGE_SIZE:(p+1)*PAGE_SIZE])[1] == PAGE_DATA)
    lp = sum(1 for p in range(np) if ph(data[p*PAGE_SIZE:(p+1)*PAGE_SIZE])[1] == PAGE_BTREE_L)
    te = sum(ph(data[p*PAGE_SIZE:(p+1)*PAGE_SIZE])[2] for p in range(np) if ph(data[p*PAGE_SIZE:(p+1)*PAGE_SIZE])[1] == PAGE_BTREE_L)
    print(f'\n{"="*55}\nSummary: {dp} data, {lp} leaf, ~{te} entries in {np} pages')

if __name__ == '__main__':
    main()
