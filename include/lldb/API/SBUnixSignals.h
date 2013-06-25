//===-- SBUnixSignals.h -----------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLDB_SBUnixSignals_h_
#define LLDB_SBUnixSignals_h_

#include "lldb/API/SBDefines.h"

namespace lldb {

class SBUnixSignals {
public:
    SBUnixSignals ();

    SBUnixSignals (const lldb::SBUnixSignals &rhs);

    ~SBUnixSignals();

    const SBUnixSignals &
    operator =(const lldb::SBUnixSignals &rhs);

    void
    Clear ();

    bool
    IsValid () const;

    const char *
    GetSignalAsCString (int32_t signo) const;

    int32_t
    GetSignalNumberFromName (const char *name) const;

    bool
    GetShouldSuppress (int32_t signo) const;

    bool
    SetShouldSuppress (int32_t signo,
                       bool value);

    bool
    GetShouldStop (int32_t signo) const;

    bool
    SetShouldStop (int32_t signo,
                   bool value);

    bool
    GetShouldNotify (int32_t signo) const;

    bool
    SetShouldNotify (int32_t signo, bool value);

    int32_t
    GetNumSignals () const;

    int32_t
    GetSignalAtIndex (int32_t index) const;

protected:
    friend class SBProcess;

    SBUnixSignals (lldb_private::UnixSignals *unix_signals_ptr);

    lldb_private::UnixSignals *
    GetPtr() const;

    void
    SetPtr (lldb_private::UnixSignals *unix_signals_ptr);

private:
    lldb_private::UnixSignals *m_opaque_ptr;
};


} // namespace lldb

#endif // LLDB_SBUnixSignals_h_
