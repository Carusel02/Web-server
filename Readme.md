# PCom Homework #4

This repository contains starter scripts for the PCom HTTP client assignment.

(for now, just a sample checker is available)

## Checker

Dependencies:

- Python >= 3.7;
- [`pexpect`](https://pexpect.readthedocs.io/en/stable/) (third party Python package);

First, install `pexpect` using PIP:
```sh
pip install --user pexpect
```

Afterwards, you can run the script on your client's compiled executable, e.g.:
```sh
# first, check the tool's internal help
python3 checker/checker.py --help 
# run the checker using default settings:
python3 checker/checker.py ../path/to/client
# Hint: supply a custom username:password (make sure to use something unique)
python3 checker/checker.py --user 'myuser-1337:hunter2' ../path/to/client
```

Also make sure to check out [the source code](./checker/checker.py) for the
actual details about the tests.

<span style="color: #A33">**Warning**: The checker is just an instrument used by
our team to automate the verification process. It should not be regarded as the
single source of truth for assessing the correctness of a solution.
It may also contain bugs (we also manually validate your programs when
a script fails), though forum reports and PRs are welcome!</span>

