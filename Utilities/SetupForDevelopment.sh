#!/usr/bin/env bash

cd "${BASH_SOURCE%/*}/.." &&
Utilities/Scripts/SetupUser.sh && echo &&
Utilities/Scripts/SetupHooks.sh && echo &&
Utilities/Scripts/SetupGitAliases.sh && echo &&
(Utilities/Scripts/SetupGerrit.sh ||
 echo 'Failed to setup Gerrit.  Run this again to retry.') && echo &&
Utilities/Scripts/GitTips.sh

# Rebase master by default
git config rebase.stat true
git config branch.master.rebase true
