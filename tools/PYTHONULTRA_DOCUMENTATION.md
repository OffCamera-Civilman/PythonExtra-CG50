# Module reference maintenance

`pythonultra_docs.py` builds one plain-text reference per public frozen module
and native module for the fx-CG50 configuration. Run the six serial runtime
preparation scripts first, in an isolated checkout; they modify target source.
The exact sequence is in `.github/workflows/module-documentation.yml`.

Then run:

```sh
python3 tools/pythonultra_docs.py --output build-module-docs/reference
```

The ZIP is written beside `reference/`. Public Python signatures and inherited
methods come from ASTs; native exports and method tables are preprocessed with
the SH port configuration. Native descriptions use checked-in MicroPython docs
with calculator corrections in `pythonultra_doc_content.py`. New public
functions without descriptions fail generation. Update examples and semantic
descriptions whenever behavior changes, even when the signature stays the same.

The reference deliberately documents compatibility no-ops and known defects.
Do not change these to desktop API promises without implementing and verifying
that behavior. The downloadable ZIP does not itself modify runtime `help()` or
`__doc__`; those remain separate tasks in `PYTHONULTRA_ROADMAP.md`.

For a local checkout whose commit metadata differs from a verified equivalent
GitHub source tree, `--revision SHA` records that verified upstream identity.
Do not use this option to label different source as an existing release.
