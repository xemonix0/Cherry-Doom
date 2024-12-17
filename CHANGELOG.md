## New Features

- **_Message Fadeout_** setting

## Changes

- Nugget's translucency features now use translucency maps from a shared pool,
  potentially improving program startup time in exchange for stutters
  when enabling said features for the first time since launch

## Bug Fixes

- Broken movement in systems assuming `char`s to be unsigned
- Message grouping only checking as many characters as the last message had,
  causing incorrect grouping (e.g. new message "TEST" being grouped with last message "TES")
