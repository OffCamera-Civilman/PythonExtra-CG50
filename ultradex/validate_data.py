#!/usr/bin/env python3
"""Offline consistency checks for UltraDex source data."""
import re
from pathlib import Path
root=Path(__file__).resolve().parent
dex=(root/'dex_data.h').read_text()
names=re.findall(r'\{"([^"]+)",\d+,\d+,\d+,\d+,\d+,\d+,\d+,\d+,\d+,\d+\}',dex)
assert len(names)==1025, f'Expected 1025 Pokemon, got {len(names)}'
assert len(set(names))==1025, 'Duplicate species names'
abilities=(root/'ability_data.h').read_text()
assert 'dex_abilities[1025]' in abilities, 'Missing abilities table'
main=(root/'main.c').read_text()
assert 'static const Move moves[]' in main, 'Missing move catalog'
moveids=[int(x) for x in re.findall(r'\{(\d+),"[^"]+",\d+,\d+,\d+,\d+,\d+,',main)]
assert len(moveids)>=900, f'Expected 900+ moves, got {len(moveids)}'
assert len(moveids)==len(set(moveids)), 'Duplicate move IDs'
learn=root/'learnset_data.h'
if learn.exists():
    s=learn.read_text()
    assert 'learn_offsets[1026]' in s, 'Missing learnset index'
    assert 'learn_generation[1025]' in s, 'Missing learnset generation'
print(f'UltraDex data OK: {len(names)} Pokemon, {len(moveids)} moves; learnsets: {"present" if learn.exists() else "not yet integrated"}')
