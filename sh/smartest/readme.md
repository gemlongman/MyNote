## smartctl


 sudo smartctl -l error  /dev/sda

### SMART overall health test

https://wiki.unraid.net/index.php/Understanding_SMART_Reports


SMART overall-health self-assessment test result: PASSED

Basic overall health test of the drive, only 2 choices - PASSED or FAILED
If test result is FAILED, then that means the SMART firmware believes that the drive is in imminent danger of catastrophic failure, so it is imperative to copy off ALL important data. Usually, it is best to copy off the most important files, then the next most important files, then the next, and so on, because the drive may completely quit before you finish copying.
SMART overall-health self-assessment test result: PASSED 