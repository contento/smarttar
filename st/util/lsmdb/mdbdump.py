#!/usr/bin/env python3
"""
MiniDB .db file inspector.

Dump (default):        mdbdump.py [-b] <path>
CSV receipts:          mdbdump.py --csv-receipts <path>
CSV statistics:        mdbdump.py --csv-stats <path>
Concise:               mdbdump.py --csv <path>

Struct offsets match BCC 3.1 16-bit DOS layout.
"""
import struct, sys, os, csv

PAGE_SIZE=512; HDR_SIZE=16
PAGE_FREE,PAGE_DBINFO,PAGE_BTREE_I,PAGE_BTREE_L,PAGE_DATA,PAGE_STATS=0,1,2,3,4,5
PNAME={0:"FREE",1:"DBINFO",2:"BTREE_I",3:"BTREE_L",4:"DATA",5:"STATS"}
RECEIPT_MAGIC=0x6719
PERIODS=['YEAR','MONTH','WEEK','DAY','TURN']

def ph(page): return struct.unpack_from('<HHHHl',page,0)

def parse_receipt(sd):
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

# DS_ENTRY ITEM offsets within the struct (BCC 3.1, no EDA):
# From:0 To:8 Tel.Nal:16  Tel.Inter:50  SpTel.Nal:84  SpTel.Inter:118
# Fax.Nal:152  Fax.Inter:186  Net.Nal:220  Net.Inter:254  Cards:288  Other:322
DS_ITEM_OFF = [16, 50, 84, 118, 152, 186, 220, 254, 288, 322]
DS_ITEM_SVC = ['TEL.Nal','TEL.Inter','SPTEL.Nal','SPTEL.Inter',
               'FAX.Nal','FAX.Inter','NET.Nal','NET.Inter','CARDS','OTHER']

def read_ds_entry(pay):
    """Parse a DS_ENTRY from page payload. Returns list of (svc, rec, talk, paid, val, tax)."""
    rows = []
    for idx, off in enumerate(DS_ITEM_OFF):
        if off + 34 > len(pay): break
        rec, tmin, pmin, v, tx = struct.unpack_from('<hdddd', pay, off)
        if rec or v:
            rows.append((DS_ITEM_SVC[idx], rec, tmin, pmin, v, tx))
    return rows

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
        if not r: out.write(f'  [slot {sl}] BAD\n'); continue
        dl='(DEL)' if r['nstat']&0x0020 else ''
        out.write(f'  [slot {sl}] #{r["number"]} booth={r["booth"]} date={r["date"]} t={r["time"]} {dl}\n')
        out.write(f'           city={r["city"]} phone={r["phone"]} amount={r["amount"]} elapsed={r["elapsed"]}s\n')
        out.write(f'           val={r["value"]:.2f} tax={r["tax"]:.2f} vpm={r["vpm"]:.4f} ceil={r["ceil"]} pct={r["percent"]}\n')

def dump_stats(page, out):
    pay = page[HDR_SIZE:]
    rows = read_ds_entry(pay)
    if not rows:
        out.write(f'  (empty)\n')
        return
    for svc, rec, tmin, pmin, v, tx in rows:
        out.write(f'  {svc:12} rec={rec} talk={tmin:.2f} paid={pmin:.2f} val={v:.2f} tax={tx:.2f}\n')

def main():
    import argparse
    parser = argparse.ArgumentParser(description='Dump MiniDB .db file contents.')
    parser.add_argument('path', nargs='?', default='RX.db')
    parser.add_argument('-b', '--brief', action='store_true')
    g = parser.add_mutually_exclusive_group()
    g.add_argument('--csv', action='store_true')
    g.add_argument('--csv-receipts', action='store_true')
    g.add_argument('--csv-stats', action='store_true')
    args = parser.parse_args()

    if not os.path.exists(args.path):
        print(f'ERROR: {args.path} not found', file=sys.stderr)
        sys.exit(1)

    with open(args.path, 'rb') as f:
        data = f.read()
    np = len(data) // PAGE_SIZE

    receipts, stats = [], []

    for pn in range(np):
        page = data[pn*PAGE_SIZE:(pn+1)*PAGE_SIZE]
        m,pt,nk,fo,r = ph(page)

        # Stats block: pages 1-7 hold DS_ENTRY data
        if 1 <= pn <= 7:
            if pn - 1 < 5:
                for svc, rec, tmin, pmin, v, tx in read_ds_entry(page[HDR_SIZE:]):
                    stats.append((PERIODS[pn-1], svc, rec, tmin, pmin, v, tx))
            continue

        if pt == PAGE_DATA:
            for sl in range(nk):
                rcp = parse_receipt(page[HDR_SIZE+sl*111:HDR_SIZE+sl*111+111])
                if rcp: receipts.append(rcp)

    # Output
    if args.csv_receipts or args.csv:
        w = csv.writer(sys.stdout)
        w.writerow(['number','booth','date','time','city','phone','amount','elapsed_s','value','tax','vpm','ceil_min','percent','deleted'])
        for r in receipts:
            w.writerow([r['number'], r['booth'], r['date'], r['time'],
                       r['city'], r['phone'], r['amount'], r['elapsed'],
                       f'{r["value"]:.2f}', f'{r["tax"]:.2f}', f'{r["vpm"]:.4f}',
                       r['ceil'], r['percent'],
                       '(deleted)' if r['nstat']&0x0020 else ''])
    if args.csv_stats or args.csv:
        if args.csv: print()
        w = csv.writer(sys.stdout)
        w.writerow(['period','service','receipts','talk_min','paid_min','value','tax'])
        for period, svc, rec, tmin, pmin, v, tx in stats:
            w.writerow([period, svc, rec, f'{tmin:.2f}', f'{pmin:.2f}', f'{v:.2f}', f'{tx:.2f}'])
    if args.csv_receipts or args.csv_stats or args.csv:
        return

    # Human-readable dump
    for pn in range(np):
        page = data[pn*PAGE_SIZE:(pn+1)*PAGE_SIZE]
        m,pt,nk,fo,r = ph(page)
        pn2 = PNAME.get(pt, f'TYPE{pt}')
        if args.brief and pt == PAGE_FREE: continue
        print(f'\n{"="*55}\nPage {pn} [{pn2}] @0x{pn*PAGE_SIZE:04x}\n{"="*55}')
        if pt == PAGE_DBINFO:  dump_dbinfo(page, sys.stdout)
        elif pt == PAGE_BTREE_L: dump_leaf(page, sys.stdout)
        elif pt == PAGE_DATA:  dump_data(page, sys.stdout)
        elif pt == PAGE_STATS: dump_stats(page, sys.stdout)
        elif pt == PAGE_FREE:  print(f'  (free sibling={r})')
        else:                  print(f'  (type {pt})')

    dp = sum(1 for p in range(np) if ph(data[p*PAGE_SIZE:(p+1)*PAGE_SIZE])[1] == PAGE_DATA)
    lp = sum(1 for p in range(np) if ph(data[p*PAGE_SIZE:(p+1)*PAGE_SIZE])[1] == PAGE_BTREE_L)
    te = sum(ph(data[p*PAGE_SIZE:(p+1)*PAGE_SIZE])[2] for p in range(np) if ph(data[p*PAGE_SIZE:(p+1)*PAGE_SIZE])[1] == PAGE_BTREE_L)
    print(f'\n{"="*55}\nSummary: {dp} data, {lp} leaf, ~{te} entries in {np} pages')

if __name__ == '__main__':
    main()
