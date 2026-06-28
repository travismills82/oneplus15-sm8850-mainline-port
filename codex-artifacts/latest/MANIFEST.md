# Canoe SCM validation handoff

- Source branch: `port/canoe-scm-v7.1-clean`
- Source commit: `81530faeabe64ba233c3ede97cc1457374f31df2`
- Pull request: `#12`
- Validation result: `invalidated`
- Merge status: `not merged`
- Reason: the published `dtbs-check.log` contains repeated `dt-validate: error: unrecognized arguments` failures.
- Root cause: two `DT_SCHEMA_FILES` filters were space-separated, but the Linux 7.1.1 make rule passes the value to `dt-validate -l`, whose multiple filters must be colon-separated.
- Additional validator defect: the kernel make rule deliberately appends `|| true` to `dt-validate`, so a zero make exit status did not prove schema validation succeeded.

A corrected validation run is required before PR #12 can be marked ready or merged.
