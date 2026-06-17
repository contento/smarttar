#!/usr/bin/env python3
"""
ls-minidb.py — dump MiniDB .db file contents.

Usage: python3 ls-minidb.py [-b] <path/to/RX.db>

Struct offsets match BCC 3.1 16-bit DOS layout.
Use -b to skip free pages.
"""
import struct, sys, os

PAGE_SIZE=512; HDR_SIZE=16
PAGE_FREE,PAGE_DBINFO,PAGE_BTREE_I,PAGE_BTREE_L,PAGE_DATA,PAGE_STATS=0,1,2,3,4,5
PNAME={0:"FREE",1:"DBINFO",2:"BTREE_I",3:"BTREE_L",4:"DATA",5:"STATS"}
RECEIPT_MAGIC=0x6719

# Receipt struct — BCC 3.1 16-bit DOS layout:
# ofs size field
#   0    2 UINT MagicNumber
#   2    4 long Number
#   6    2 enum TTag
#   8    2 union Stat/nStat (UINT)
#  10    1 UCHAR bExtendedStat
#  11    2 int Date
#  13    2 int Time
#  15    2 int BoothNumber
#  17   21 char City[21]
#  38   17 char Phone[17]
#  55    2 int Amount
#  57    4 long ElapsedTime
#  61    8 double ValuePerMin
#  69    8 double CeilMin
#  77    2 int Percent
#  79    8 double Value
#  87    8 double Tax
#  95    8 double Tax2
# 103    8 double DDummy
# total 111

def ph(page): return struct.unpack_from('<HHHHl',page,0)

def dump_dbinfo(page):
    m,pt,nk,fo,r=ph(page)
    print(f'  Magic=0x{m:04X}  Type={PNAME.get(pt,str(pt))}')
    if m!=0xDBDB or pt!=PAGE_DBINFO: print('  *** INVALID ***'); return
    ver,root,seq,nrecs,narc,sa,fh=struct.unpack_from('<H6l',page,HDR_SIZE)
    print(f'  Version={ver}  RootPage={root}  Sequence={seq}')
    print(f'  NumReceipts={nrecs}  NumArchived={narc}  StatsAnchor={sa}  FreeHead={fh}')

def dump_leaf(page):
    m,pt,nk,fo,r=ph(page)
    if nk==0: return
    print(f'  NumKeys={nk}  RightSibling={r}')
    for i in range(nk):
        off=HDR_SIZE+i*12
        num,booth,sp,fl=struct.unpack_from('<lhlH',page,off)
        pn=sp>>8; sl=sp&3; d='(del)' if fl&1 else ''
        print(f'  [{i}] num={num} booth={booth} page={pn} slot={sl} {d}')

def dump_data(page):
    m,pt,nk,fo,r=ph(page)
    if nk==0: return
    print(f'  NumKeys={nk}')
    for sl in range(nk):
        off=HDR_SIZE+sl*111; sd=page[off:off+111]
        if len(sd)<111: break
        mn=struct.unpack_from('<H',sd,0)[0]
        if mn!=RECEIPT_MAGIC: print(f'  [slot {sl}] BAD magic=0x{mn:04X}'); continue
        num=struct.unpack_from('<l',sd,2)[0]
        tag=struct.unpack_from('<H',sd,6)[0]
        sr=struct.unpack_from('<H',sd,8)[0]
        date=struct.unpack_from('<h',sd,11)[0]
        rtime=struct.unpack_from('<h',sd,13)[0]
        booth=struct.unpack_from('<h',sd,15)[0]
        city=sd[17:38].split(b'\0')[0].decode('latin-1',errors='replace')
        phone=sd[38:55].split(b'\0')[0].decode('latin-1',errors='replace')
        amt=struct.unpack_from('<h',sd,55)[0]
        elap=struct.unpack_from('<l',sd,57)[0]
        vpm=struct.unpack_from('<d',sd,61)[0]
        ceil=struct.unpack_from('<d',sd,69)[0]
        pct=struct.unpack_from('<h',sd,77)[0]
        val=struct.unpack_from('<d',sd,79)[0]
        tx=struct.unpack_from('<d',sd,87)[0]
        tx2=struct.unpack_from('<d',sd,95)[0]
        dl='(DEL)' if sr&0x0020 else ''
        print(f'  [slot {sl}] #{num} booth={booth} date={date} t={rtime} {dl}')
        print(f'           city={city} phone={phone} amount={amt} elapsed={elap}s')
        print(f'           val={val:.2f} tax={tx:.2f} vpm={vpm:.4f} ceil={ceil} pct={pct}')

def dump_stats(page):
    m,pt,nk,fo,r=ph(page)
    print(f'  NumKeys={nk}')
    periods=['YEAR','MONTH','WEEK','DAY','TURN']
    svcs=['TEL','SPECIAL_TEL','FAX','TELEX','CARD','OTHER']
    pay=page[HDR_SIZE:]
    for p in range(5):
        print(f'  --- {periods[p]} ---')
        for s in range(6):
            off=(p*6+s)*30
            if off+30>len(pay): break
            rec,tmin,pmin,v,tx=struct.unpack_from('<hdddd',pay,off)
            if rec or v:
                print(f'    {svcs[s]:12} rec={rec} talk={tmin:.2f} paid={pmin:.2f} val={v:.2f} tax={tx:.2f}')

def main():
    brief='-b' in sys.argv
    paths=[a for a in sys.argv[1:] if not a.startswith('-')]
    if not paths: print(f'Usage: {sys.argv[0]} [-b] <path>'); sys.exit(1)
    for path in paths:
        if not os.path.exists(path): print(f'ERROR: {path}'); continue
        with open(path,'rb') as f: data=f.read()
        np=len(data)//PAGE_SIZE
        print(f'\nFile: {path} ({len(data)} bytes, {np} pages)')
        dp=lp=te=0
        for pn in range(np):
            page=data[pn*PAGE_SIZE:(pn+1)*PAGE_SIZE]
            m,pt,nk,fo,r=ph(page)
            pn2=PNAME.get(pt,f'TYPE{pt}')
            if brief and pt==PAGE_FREE: continue
            print(f'\n{"="*55}\nPage {pn} [{pn2}] @0x{pn*PAGE_SIZE:04x}\n{"="*55}')
            if pt==PAGE_DBINFO: dump_dbinfo(page)
            elif pt==PAGE_BTREE_L: dump_leaf(page); lp+=1; te+=nk
            elif pt==PAGE_DATA: dump_data(page); dp+=1
            elif pt==PAGE_STATS: dump_stats(page)
            elif pt==PAGE_FREE: print(f'  (free sibling={r})')
            else: print(f'  (type {pt})')
        print(f'\n{"="*55}\nSummary: {dp} data, {lp} leaf, ~{te} entries')

if __name__=='__main__': main()
