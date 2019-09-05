========================================================================
    DYNAMIC LINK LIBRARY : SkyRadio Project Overview
========================================================================

Full support of Silabs Si470x chip working in Windows enviroment. Project was build with VS2010.
Support of:
1. Radio Sound (via DirectSound)
2. Radio basic functions (seek, volume, ecc)
3. Radio Basic RDS (PTY, TA, AF list, ecc)
4. Advanced RDS (Eon, TP, TMC)
4a. It fully decrypts all Radio Registers in partically the 8A register where TMC data gets injected. So we can collect and forward traffic alerts to other applications (navigators and such). It supports encrypted events too although the table is not available (standing to RDS standard it is a premium service)

