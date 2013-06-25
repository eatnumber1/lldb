//===-- SBUnixSignals.cpp -------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "lldb/lldb-defines.h"
#include "lldb/Target/UnixSignals.h"
#include "lldb/Core/Log.h"

#include "lldb/API/SBUnixSignals.h"

using namespace lldb;
using namespace lldb_private;

SBUnixSignals::SBUnixSignals () :
    m_opaque_ptr(NULL)
{
}

SBUnixSignals::SBUnixSignals (const SBUnixSignals &rhs) :
    m_opaque_ptr(rhs.m_opaque_ptr)
{
}

SBUnixSignals::SBUnixSignals (UnixSignals *unix_signals_ptr) :
    m_opaque_ptr(unix_signals_ptr)
{
}

const SBUnixSignals&
SBUnixSignals::operator = (const SBUnixSignals& rhs)
{
    if (this != &rhs)
        m_opaque_ptr = rhs.m_opaque_ptr;
    return *this;
}

SBUnixSignals::~SBUnixSignals()
{
}

UnixSignals *
SBUnixSignals::GetPtr() const
{
    return m_opaque_ptr;
}

void
SBUnixSignals::SetPtr (UnixSignals *unix_signals_ptr)
{
    m_opaque_ptr = unix_signals_ptr;
}

void
SBUnixSignals::Clear ()
{
    m_opaque_ptr = NULL;
}

bool
SBUnixSignals::IsValid() const
{
    return m_opaque_ptr != NULL;
}

const char *
SBUnixSignals::GetSignalAsCString (int32_t signo) const
{
    if (m_opaque_ptr) return m_opaque_ptr->GetSignalAsCString(signo);
    return NULL;
}

int32_t
SBUnixSignals::GetSignalNumberFromName (const char *name) const
{
    if (m_opaque_ptr) return m_opaque_ptr->GetSignalNumberFromName(name);
    return -1;
}

bool
SBUnixSignals::GetShouldSuppress (int32_t signo) const
{
    if (m_opaque_ptr) return m_opaque_ptr->GetShouldSuppress(signo);
    return false;
}

bool
SBUnixSignals::SetShouldSuppress (int32_t signo, bool value)
{
    Log *log(lldb_private::GetLogIfAllCategoriesSet (LIBLLDB_LOG_API));

    if (log)
    {
        log->Printf ("SBUnixSignals(%p)::SetShouldSuppress (signo=%d, value=%d)",
                     m_opaque_ptr,
                     signo,
                     value);
    }

    if (m_opaque_ptr) return m_opaque_ptr->SetShouldSuppress(signo, value);
    return false;
}

bool
SBUnixSignals::GetShouldStop (int32_t signo) const
{
    if (m_opaque_ptr) return m_opaque_ptr->GetShouldStop(signo);
    return false;
}

bool
SBUnixSignals::SetShouldStop (int32_t signo, bool value)
{
    Log *log(lldb_private::GetLogIfAllCategoriesSet (LIBLLDB_LOG_API));

    if (log)
    {
        log->Printf ("SBUnixSignals(%p)::SetShouldStop (signo=%d, value=%d)",
                     m_opaque_ptr,
                     signo,
                     value);
    }

    if (m_opaque_ptr) return m_opaque_ptr->SetShouldStop(signo, value);
    return false;
}

bool
SBUnixSignals::GetShouldNotify (int32_t signo) const
{
    if (m_opaque_ptr) return m_opaque_ptr->GetShouldNotify(signo);
    return false;
}

bool
SBUnixSignals::SetShouldNotify (int32_t signo, bool value)
{
    Log *log(lldb_private::GetLogIfAllCategoriesSet (LIBLLDB_LOG_API));

    if (log)
    {
        log->Printf ("SBUnixSignals(%p)::SetShouldNotify (signo=%d, value=%d)",
                     m_opaque_ptr,
                     signo,
                     value);
    }

    if (m_opaque_ptr) return m_opaque_ptr->SetShouldNotify(signo, value);
    return false;
}

int32_t
SBUnixSignals::GetNumSignals () const
{
    if (m_opaque_ptr)
    {
        int32_t num_signals = 0;
        for (
            int32_t signo = m_opaque_ptr->GetFirstSignalNumber();
            signo != LLDB_INVALID_SIGNAL_NUMBER;
            signo = m_opaque_ptr->GetNextSignalNumber(signo)
        )
        {
            num_signals++;
        }
        return num_signals;
    }
    return LLDB_INVALID_SIGNAL_NUMBER;
}

int32_t
SBUnixSignals::GetSignalAtIndex (int32_t index) const
{
    if (m_opaque_ptr)
    {
        int32_t idx = 0;
        for (
            int32_t signo = m_opaque_ptr->GetFirstSignalNumber();
            signo != LLDB_INVALID_SIGNAL_NUMBER;
            signo = m_opaque_ptr->GetNextSignalNumber(signo)
        )
        {
            if (index == idx) return signo;
            idx++;
        }
    }
    return LLDB_INVALID_SIGNAL_NUMBER;
}
