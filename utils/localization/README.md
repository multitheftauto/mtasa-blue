These scripts will run in a native Python 3 environment. You can install from the website or appstore as appropriate.

# Setting up development environment

For testing, a Conda environment is provided. This is the recommended distribution for installing Python on Windows.

## Step 1: Download Miniconda3

Download available here: https://docs.conda.io/en/latest/miniconda.html

## Step 2: Open Terminal

- **Windows**: Open `Anaconda Powershell Prompt` from your start menu
- **Linux**: Just open any Terminal

Then `cd` into your `mtasa-blue` git directory

## Step 3: Create environment

In the shell, type

`conda env create -f utils/localization/environment.yml`

# Running scripts

## Step 1: Activate environment

In shell

`conda activate mtasa`

## Step 2: Run scripts

`python utils/localization/build_gettext_catalog.py --help`

# Running tests

```shell
conda activate mtasa
pytest utils/localization/tests
```
