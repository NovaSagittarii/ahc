# AHC035 - Breed Improvement

required things
get the tests first and dump them inside in

```sh
# init
python3 -m venv .venv
source .venv/bin/activate
pip install neat-python
```

```sh
# run once
make build/jury && ./build/jury < in/0000.txt > o.txt

# run many, print aggregate
python3 testbench.py
```