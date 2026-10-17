"""Generate readable indexes and validate collected archive metadata/source patch."""
from pathlib import Path
import os,sys,json,hashlib,subprocess,tempfile,datetime
WS=Path(r'C:\Users\box3\Documents\ChatGPT\ps3');OUT=Path(r'C:\Users\box3\Desktop\Twisted Metal - Gesamtdossier')
AUDIT=OUT/'05_PRUEFSUMMEN';SOURCE=OUT/'03_QUELLCODE'
if not (AUDIT/'ASSEMBLY_RESULT.json').exists():raise RuntimeError('Assembly not complete')
report=json.loads((AUDIT/'ASSEMBLY_RESULT.json').read_text(encoding='utf-8'));rows=json.loads((AUDIT/'COLLECTED_FILES.json').read_text(encoding='utf-8'))
def lp(p):
 s=os.path.abspath(str(p));return s if s.startswith('\\\\?\\') else '\\\\?\\'+s
for r in rows:
 p=OUT/r['file']
 if not os.path.isfile(lp(p)) or os.stat(lp(p)).st_size!=r['bytes']:raise RuntimeError('Missing/size mismatch: '+str(p))
index=json.loads((AUDIT/'CHECKPOINT_INDEX.json').read_text(encoding='utf-8'))
lines=['# Checkpoint-Verzeichnis','',f'{len(index)} Checkpointordner in dieser Sammlung. Originaldateien bleiben in ihrer Struktur erhalten.','']
for item in sorted(index,key=lambda x:x['checkpoint']):
 lines+=['## '+item['checkpoint'],'']
 docs=item['documents']
 preferred=[p for p in docs if Path(p).name.lower() in {'readme.md','status.md','user-result.md','report.md'}]
 if not preferred:preferred=docs[:4]
 for p in preferred:lines+=['- ['+p.split('/')[-1]+']('+p.replace(' ','%20')+')']
 if not docs:lines+=['- Quelldateien/Metadaten/Buildbelege vorhanden; keine Markdown-Zusammenfassung in diesem Ordner.']
 lines+=['']
(OUT/'CHECKPOINT_VERZEICHNIS.md').write_text('\n'.join(lines),encoding='utf-8')
# Independent temporary Git index; do not touch the real index or working tree.
with tempfile.TemporaryDirectory(prefix='tm-dossier-index-') as tmp:
 env=os.environ.copy();env['GIT_INDEX_FILE']=str(Path(tmp)/'index')
 def run(args):return subprocess.run(['git','-C',str(WS/'rpcs3-live-4k')]+args,env=env,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
 a=run(['read-tree',report['source_base']]);
 if a.returncode:raise RuntimeError(a.stderr.decode(errors='replace'))
 p=run(['apply','--cached','--check','--whitespace=nowarn',str(SOURCE/'tracked-changes.patch')])
 patch_result=dict(result='PASS' if p.returncode==0 else 'FAIL',base=report['source_base'],exit_code=p.returncode,stdout=p.stdout.decode(errors='replace'),stderr=p.stderr.decode(errors='replace'),real_index_unchanged=True,clean_compile_performed=False)
(AUDIT/'PATCH_APPLIES_TO_BASE.json').write_text(json.dumps(patch_result,indent=2),encoding='utf-8')
if p.returncode:raise RuntimeError('Source patch base check failed')
# Capture listing usable by a human, alongside machine-readable complete inventory.
caps=json.loads((AUDIT/'CAPTURES.json').read_text(encoding='utf-8'))
cl=['# Gesicherte RenderDoc-Aufnahmen','',f'{len(caps)} RDC-Dateien. Alle beim Sammeln vorhandenen Inhalte geprüft; nicht alle neu replayt.','']
for r in caps:cl+=['- `'+r['file']+'` — '+str(r['bytes'])+' Bytes, SHA256 `'+r['sha256']+'`']
(OUT/'CAPTURE_VERZEICHNIS.md').write_text('\n'.join(cl),encoding='utf-8')
summary=['# Ergebnis der Sammlung','',f"Dateien aus Quellsammlungen: **{report['files']}**.",f"Checkpointordner: **{report['checkpoint_folders']}**.",f"RDC-Dateien einschließlich historischer Kopien: **{report['captures']}**.",f"Logische Gesamtgröße: **{report['logical_bytes']/1e9:.2f} GB** (nicht zusätzlicher Speicherverbrauch).",f"Platzsparende große historische Hardlinks: **{report['hardlinked_files']}**.",'','Alle erfassten Quell-/Zieldateien beim Sammeln perSHA256 abgeglichen; anschließend Existenz/Größe allerEinträge erneut geprüft.','Quellpatch lässt sich im getrennten temporärenGitindex gegen festgehalteneBasis anwenden. Kein CleanBuild/Spieltest hier.','','Releaseidentität beim Sammeln:','']
for r in report['release_snapshots']:summary+=['- `'+r['name']+'`: `'+r['exe_sha256']+'`']
summary+=['','Ausgelassene Verzeichnisverknüpfungen/fehlende Quellen: sieheSKIPPED_AND_EXTERNAL.json; keine blinde Rekursion in gemeinsameLaufzeitordner.','PrivatesGesamtarchiv, nicht komplett veröffentlichen. Dateien über die spätere GitHub-Auswahl gesondert freigeben.']
(OUT/'SAMMLUNGSERGEBNIS.md').write_text('\n'.join(summary),encoding='utf-8')
def sha(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for b in iter(lambda:f.read(1024*1024),b''):h.update(b)
 return h.hexdigest().upper()
extra=[]
for folder in [OUT,SOURCE,OUT/'06_ZUSATZBELEGE']:
 for q in folder.rglob('*') if folder!=OUT else folder.iterdir():
  if q.is_file():extra.append(dict(file=q.relative_to(OUT).as_posix(),bytes=q.stat().st_size,sha256=sha(q)))
(AUDIT/'ADDENDUM_FILES.json').write_text(json.dumps(extra,indent=2,ensure_ascii=False),encoding='utf-8')
verification=dict(result='PASS',collection_files_checked=len(rows),metadata_check='all collected files exist and sizes match; full SHA256 equality checked during assembly',source_patch_base_check='PASS',supplementary_files=len(extra),clean_build_performed=False,game_started=False)
(AUDIT/'FINAL_VERIFICATION.json').write_text(json.dumps(verification,indent=2),encoding='utf-8')
print(json.dumps(dict(assembly=report,final_verification=verification),indent=2))
