"""Private evidence collection. No game/build changes; generated manifests and git artifacts."""
from pathlib import Path
import os,sys,json,hashlib,shutil,subprocess,datetime,time,stat
WS=Path(r'C:\Users\box3\Documents\ChatGPT\ps3')
DESKTOP=Path(r'C:\Users\box3\Desktop')
OUT=DESKTOP/'Twisted Metal - Gesamtdossier'
CP=WS/'checkpoints/2026-10-16-project-dossier'
CP.mkdir(exist_ok=True)
if OUT.exists():raise RuntimeError('Refusing existing dossier destination')
OUT.mkdir()
ROWS=[];SKIPS=[];DIAGNOSTICS=[];CACHE={};START=time.time()
HARD_EXT={'.rdc','.raw','.bin','.exe','.dll','.zip','.7z','.bak','.png','.jpg','.jpeg','.psarc','.vram','.dram','.ltn','.ngp'}
def lp(p):
 s=os.path.abspath(str(p));return s if s.startswith('\\\\?\\') else '\\\\?\\'+s
def sha(p):
 st=os.stat(lp(p));key=(st.st_dev,st.st_ino,st.st_size,st.st_mtime_ns)
 if key not in CACHE:
  h=hashlib.sha256()
  with open(lp(p),'rb') as f:
   for b in iter(lambda:f.read(8*1024*1024),b''):h.update(b)
  CACHE[key]=h.hexdigest().upper()
 return CACHE[key]
def copy_file(src,dst,independent=False):
 src,dst=Path(src),Path(dst)
 if dst.exists():raise RuntimeError('Duplicate destination: '+str(dst))
 before=os.stat(lp(src));os.makedirs(lp(dst.parent),exist_ok=True)
 method='COPY'
 if not independent and before.st_size>=8*1024*1024 and src.suffix.lower() in HARD_EXT:
  try:os.link(lp(src),lp(dst));method='HARDLINK'
  except OSError:shutil.copy2(lp(src),lp(dst))
 else:shutil.copy2(lp(src),lp(dst))
 after=os.stat(lp(src));target=os.stat(lp(dst))
 if (before.st_size,before.st_mtime_ns)!=(after.st_size,after.st_mtime_ns):raise RuntimeError('Source changed while collecting: '+str(src))
 h=sha(src)
 if target.st_size!=after.st_size or sha(dst)!=h:raise RuntimeError('Destination hash mismatch: '+str(dst))
 ROWS.append(dict(source=str(src),file=dst.relative_to(OUT).as_posix(),bytes=after.st_size,sha256=h,method=method))
 if len(ROWS)%300==0:print(json.dumps(dict(event='progress',files=len(ROWS),logical_gb=round(sum(r['bytes'] for r in ROWS)/1e9,2),seconds=round(time.time()-START))),flush=True)
def copy_tree(src,dst,independent=False):
 src,dst=Path(src),Path(dst)
 if not src.exists():SKIPS.append(dict(source=str(src),reason='not present'));return
 stack=[src]
 while stack:
  parent=stack.pop()
  with os.scandir(lp(parent)) as it:entries=list(it)
  for e in entries:
   p=parent/e.name
   attrs=e.stat(follow_symlinks=False).st_file_attributes
   if attrs&stat.FILE_ATTRIBUTE_REPARSE_POINT:
    SKIPS.append(dict(source=str(p),reason='reparse point not traversed',target=os.readlink(lp(p)) if e.is_symlink() else 'junction'));continue
   rel=p.relative_to(src);q=dst/rel
   if e.is_dir(follow_symlinks=False):stack.append(p)
   else:copy_file(p,q,independent)
def git(args):
 p=subprocess.run(['git','-C',str(WS/'rpcs3-live-4k')]+args,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
 if p.returncode:raise RuntimeError(p.stderr.decode(errors='replace'))
 if p.stderr:DIAGNOSTICS.append(p.stderr.decode(errors='replace'))
 return p.stdout
# Complete historical checkpoints and evidence roots. No traversal into runtime junctions.
for name in ['checkpoints','captures','postfx-120min','postfx-code-analysis','renderpfad-agent','spu-cache-analysis','texture-upscale-experiment','publication-evidence','twisted-extracted','renderdoc_docs','launchers']:
 print('Collecting workspace/'+name,flush=True)
 copy_tree(WS/name,OUT/'01_WORKSPACE'/name)
for p in WS.iterdir():
 if p.is_file():copy_file(p,OUT/'01_WORKSPACE'/p.name)
for name in ['Aufgabenzettel','Übergabe Twisted Metal','Gemini Twisted Metal','Gemini Twisted Metal Bildauswertung','Gemini Farben Master Vorarbeit','twisted metal gemini','TWgemini','Twisted Metal 4K Final Master Bilder','Twisted Metal 4K Texturen-Analyse','Twisted Metal - Archiv']:
 print('Collecting Desktop/'+name,flush=True)
 copy_tree(DESKTOP/name,OUT/'02_DESKTOP_SAMMLUNGEN'/name)
# Exact RPCS3 source delta and nontracked header files against the pinned base.
source=OUT/'03_QUELLCODE';source.mkdir()
for name,args in [('BASE_COMMIT.txt',['rev-parse','HEAD']),('git-status.txt',['status','--short']),('tracked-changes.patch',['diff','--binary','--no-ext-diff','HEAD']),('diff-stat.txt',['diff','--stat','HEAD']),('submodules.txt',['submodule','status','--recursive'])]:
 (source/name).write_bytes(git(args))
changed=git(['diff','--name-only','-z','HEAD']).decode().split('\0')
untracked=git(['ls-files','--others','--exclude-standard','-z']).decode().split('\0')
for rel in dict.fromkeys(changed+untracked):
 if rel:copy_file(WS/'rpcs3-live-4k'/rel,source/'overlay'/rel,True)
for name in ['BUILDING.md','LICENSE','README.md','.gitmodules','CMakeLists.txt','CMakePresets.json']:
 p=WS/'rpcs3-live-4k'/name
 if p.exists():copy_file(p,source/'build-reference'/name,True)
for name in ['CMakeCache.txt','rpcs3/Emu/rpcs3_emu.vcxproj','rpcs3/rpcs3.vcxproj']:
 p=WS/'rpcs3-live-4k/build-msvc2026'/name
 if p.exists():copy_file(p,source/'local-build-reference'/name,True)
# Snapshot currently documented Gemini release candidates, not all runtime caches.
release=[];base=WS/'rpcs3-live-4k/build-msvc2026'
for name in ['bin-twisted-metal-4k-sichtweite-test','bin-twisted-metal-4k-stabilitats-test','bin-twisted-metal-4k-echte-dichte']:
 src=base/name;dst=OUT/'04_RELEASE_SNAPSHOTS'/name
 if not src.exists():SKIPS.append(dict(source=str(src),reason='documented candidate absent'));continue
 print('Snapshot candidate '+name,flush=True)
 for p in src.iterdir():
  if p.is_file() and (p.suffix.lower() in {'.exe','.dll','.flag'}):copy_file(p,dst/p.name,True)
 for part in ['config','patches','GuiConfigs','log']:
  copy_tree(src/part,dst/part,True)
 for part in ['levels','mplevels']:
  folder=src/'dev_hdd0/game/BCUS98106/USRDIR'/part
  for filename in ['times.ltn','suburbs.ltn','times.psarc','suburbs.psarc']:
   p=folder/filename
   if p.is_file():copy_file(p,dst/'private-game-assets'/part/filename,True)
 release.append(dict(name=name,path=str(src),exe_sha256=sha(src/'rpcs3.exe')))
# Lightweight inventory of all live runtime versions; do not copy their caches/saves/firmware.
runtime=[]
for d in base.iterdir():
 if d.is_dir() and d.name.startswith('bin') and (d/'rpcs3.exe').is_file():runtime.append(dict(name=d.name,exe=str(d/'rpcs3.exe'),sha256=sha(d/'rpcs3.exe')))
(OUT/'04_RELEASE_SNAPSHOTS/runtime-inventory.json').write_text(json.dumps(runtime,indent=2),encoding='utf-8')
(OUT/'04_RELEASE_SNAPSHOTS/candidate-hashes.json').write_text(json.dumps(release,indent=2),encoding='utf-8')
# Audit: source references and all collected file hashes; no missing input hidden.
audit=OUT/'05_PRUEFSUMMEN';audit.mkdir()
(audit/'COLLECTED_FILES.json').write_text(json.dumps(ROWS,indent=2,ensure_ascii=False),encoding='utf-8')
(audit/'SKIPPED_AND_EXTERNAL.json').write_text(json.dumps(SKIPS,indent=2,ensure_ascii=False),encoding='utf-8')
(audit/'git-diagnostics.txt').write_text('\n'.join(DIAGNOSTICS),encoding='utf-8')
captured=[r for r in ROWS if Path(r['file']).suffix.lower()=='.rdc']
(audit/'CAPTURES.json').write_text(json.dumps(captured,indent=2,ensure_ascii=False),encoding='utf-8')
index=[]
for p in (WS/'checkpoints').iterdir():
 if p.is_dir():
  docs=[r['file'] for r in ROWS if r['file'].startswith('01_WORKSPACE/checkpoints/'+p.name+'/') and Path(r['file']).suffix.lower()=='.md']
  index.append(dict(checkpoint=p.name,documents=docs))
(audit/'CHECKPOINT_INDEX.json').write_text(json.dumps(index,indent=2,ensure_ascii=False),encoding='utf-8')
report=dict(result='PASS',date='2026-10-16',files=len(ROWS),logical_bytes=sum(r['bytes'] for r in ROWS),hardlinked_files=sum(r['method']=='HARDLINK' for r in ROWS),copied_files=sum(r['method']=='COPY' for r in ROWS),captures=len(captured),checkpoint_folders=len(index),source_base=git(['rev-parse','HEAD']).decode().strip(),release_snapshots=release,skips=len(SKIPS),elapsed_seconds=round(time.time()-START),all_collected_files_sha256_verified=True,clean_rebuild_performed=False,game_started=False,private_not_for_public_upload=True)
(audit/'ASSEMBLY_RESULT.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
(CP/'assembly-result.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
print(json.dumps(report,indent=2),flush=True)
