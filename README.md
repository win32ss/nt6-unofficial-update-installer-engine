# nt6-unofficial-update-installer-engine
Installer engine that takes care of what is needed to install unofficial system files. It was created for the benefit of the Vista Extended Kernel and Media Foundation Update, but it can adaptable to other uses. 

It is presently capable of obtaining lists of files to be copied into System32 and SysWOW64 folders, checking minimum and maximum build numbers via the PEB, automatically backing up replaced system files, and offline backup restoration. Privilege escalation is enforced.

Feature additions, such as a GUI front-end, covering a broader variety of file paths, and the ability to specify other "jobs" for the installer to run, are possible in the future.

A sample syntax for the configuration file can be found in the posted "setup.conf".
