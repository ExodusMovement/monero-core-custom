## Upgrading to upstream

### Patches folder

Patches folder should contain desired version of upstream files but with patches applied.

- common/perf_timer.h - stripped unused code
- util.cpp util.h - stripped unused code
- crypto - dummy RandomX and CN implementation
- account.h/account.cpp - `from_legacy16B_lw_seed support`
- cryptonote_basic_impl.cpp - remove `get_account_address_from_str_or_url`
- cryptonote_format_utils.cpp - remove `hwdev.compute_key_image` from `generate_key_image_helper_precomp`
- cryptonote_tx_utils.cpp/h - remove `Blockchain usage`, comment out `construct_miner_tx`
- misc_log_ex.h - add `#include <sstream>`, remove logging functionality.

### v0.14.0.0 (f96e431a15c510db4511f2ed6fe5ee5ab708a755) -> v0.15.0.0 (69c488a479609df2838c14cd0cf500242758f449)

#### Before upgrade

```sh
./upgrade.sh prepare_v0_15_0_0
```

#### Upgrade

```sh
./upgrade.sh patch_and_fix
```
