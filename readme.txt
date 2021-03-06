tVolEzy - The easy to use volume changing module written by Tobbe
=================================================================

This module is supposed to control the main system volume using simple bang
commands. It's for those of you who just want to control your volume and
don't need to display a slider or anything like that.

The module has only been tested on Windows XP and Windows 7, but should work on
anything from Windows 2000 and up.


Installing and loading the module
---------------------------------

Classic Setup:

    Put tVolEzy.dll in a directory of your choice, e.g.
    C:\Program Files\Litestep\Modules\. Open your step.rc and add a LoadModule
    line pointing to the dll, for example:

    LoadModule "C:\Program Files\Litestep\Modules\tVolEzy.dll"

NetLoadModule (OTS2) Setup:

    Add the following line to your step.rc/theme.rc/personal.rc/whatever:

    *NetLoadModule tVolEzy-2.0

Then save the file you just edited and recycle LiteStep.


.rc settings and commands
-------------------------

tVolEzyShowErrors true
    Setting this to 'false' will disable all error message boxes.
    Default is 'true'.

tVolEzyUnmuteOnVolUp true
    Makes tVolEzy unmute the sound when you call !tVolEzyUp
    Default is 'true'

tVolEzyUnmuteOnVolDown false
    Makes tVolEzy unmute the sound when you call !tVolEzyDown
    Default is 'false'


!bang commands
--------------

!tVolEzyUp 1
    Increases the volume the specified amount of steps. Minimum volume is 0 and
    full volume is 100.
    The parameter is optional and defaults to 1 if not specified.

!tVolEzyDown 1
    Decreases the volume the specified amount of steps. Minimum volume is 0 and
    full volume is 100.
    The parameter is optional and defaults to 1 if not specified.

!tVolEzyToggleMute
    Use this !bang to mute or unmute the sound. !tVolEzyToggleMute mutes the
    system sound when it isn't muted and unmutes it when it is muted.

!tVolEzyVolumeChangedCommand
    Sets a command to be executed when the system volume changes. #VOLUME# will
    be replaced by the actual system volume.
    Example: tVolEzyVolumeChangedCommand "!CommandSetText New Volume: #VOLUME#"
    A parameter must be supplied, there is no default.

!tVolEzyMuteChangedCommand
    Sets a commadn to be executed when the system mute status changes. #MUTE"
    will be replaced by "muted" or "unmuted" without the quotes.
    Example: tVolEzyMuteChangedCommand "!CommandSetText Mute Changed: #MUTE#"
    A parameter must be supplied, there is no default.


Version history
---------------

Tobbe 2010-02-13 tVolEzy 2.0
    Added volume and mute change notification.

Tobbe 2009-05-17 tVolEzy 1.0
    Added support for Windows Vista and newer.

Tobbe 2009-05-01 tVolEzy 0.1
    Released the first version, 0.1.
