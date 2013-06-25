"""
Test SBProcess APIs, including ReadMemory(), WriteMemory(), and others.
"""

import os, time
import unittest2
import lldb
from lldbutil import get_stopped_thread, state_type_to_str
from lldbtest import *

class ProcessAPITestCase(TestBase):
    mydir = os.path.join("python_api", "signals")

    @python_api_test
    def test_ignore_signal(self):
        """Test Python SBUnixSignals.Suppress/Stop/Notify() API."""
        self.buildDefault()
        exe = os.path.join(os.getcwd(), "a.out")
        self.runCmd("file " + exe, CURRENT_EXECUTABLE_SET)

        target = self.dbg.CreateTarget(exe)
        self.assertTrue(target, VALID_TARGET)

        error = lldb.SBError()
        event = lldb.SBEvent();
        listener = lldb.SBListener("TestSignalsAPI Listener")
        # Launch the process, and stop at the entry point.
        process = target.Launch(listener, None, None,
                                None, None, None,
                                os.getcwd(), 0, True, error)
        self.assertTrue(error.Success(), error.GetCString())

        while not event.IsValid() or not lldb.SBProcess.GetStateFromEvent(event) == lldb.eStateStopped:
            self.assertTrue(listener.WaitForEvent(3, event), "Listener timeout")

        unix_signals = process.GetUnixSignals()
        sigint = unix_signals.GetSignalNumberFromName("SIGINT")
        unix_signals.SetShouldSuppress(sigint, True)
        unix_signals.SetShouldStop(sigint, False)
        unix_signals.SetShouldNotify(sigint, False)

        process.Continue()
        self.assertTrue(process.state == lldb.eStateExited, "The process should have exited")
        self.assertTrue(process.GetExitStatus() == 0, "The process should have returned 0")


if __name__ == '__main__':
    import atexit
    lldb.SBDebugger.Initialize()
    atexit.register(lambda: lldb.SBDebugger.Terminate())
    unittest2.main()
