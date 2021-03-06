//===-- DWARFDebugLine.h ----------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef SymbolFileDWARF_DWARFDebugLine_h_
#define SymbolFileDWARF_DWARFDebugLine_h_

#include <map>
#include <vector>
#include <string>

#include "lldb/lldb-private.h"

#include "DWARFDataExtractor.h"
#include "DWARFDefines.h"

class SymbolFileDWARF;
class DWARFDebugInfoEntry;

//----------------------------------------------------------------------
// DWARFDebugLine
//----------------------------------------------------------------------
class DWARFDebugLine
{
public:
    //------------------------------------------------------------------
    // FileNameEntry
    //------------------------------------------------------------------
    struct FileNameEntry
    {
        FileNameEntry() :
            name(),
            dir_idx(0),
            mod_time(0),
            length(0)
        {
        }

        std::string     name;
        dw_sleb128_t    dir_idx;
        dw_sleb128_t    mod_time;
        dw_sleb128_t    length;

    };

    //------------------------------------------------------------------
    // Prologue
    //------------------------------------------------------------------
    struct Prologue
    {

        Prologue() :
            total_length(0),
            version(0),
            prologue_length(0),
            min_inst_length(0),
            default_is_stmt(0),
            line_base(0),
            line_range(0),
            opcode_base(0),
            standard_opcode_lengths(),
            include_directories(),
            file_names(),
            is_64_bit(false)
        {
        }

        typedef std::shared_ptr<Prologue> shared_ptr;

        uint32_t    total_length;   // The size in bytes of the statement information for this compilation unit (not including the total_length field itself).
        uint16_t    version;        // Version identifier for the statement information format.
        uint32_t    prologue_length;// The number of bytes following the prologue_length field to the beginning of the first byte of the statement program itself.
        uint8_t     min_inst_length;// The size in bytes of the smallest target machine instruction. Statement program opcodes that alter the address register first multiply their operands by this value.
        uint8_t     default_is_stmt;// The initial value of theis_stmtregister.
        int8_t      line_base;      // This parameter affects the meaning of the special opcodes. See below.
        uint8_t     line_range;     // This parameter affects the meaning of the special opcodes. See below.
        uint8_t     opcode_base;    // The number assigned to the first special opcode.
        std::vector<uint8_t>            standard_opcode_lengths;
        std::vector<std::string>        include_directories;
        std::vector<FileNameEntry>      file_names;

        bool        is_64_bit;      // 64-bit dwarf
        uint32_t SizeofTotalLength() const { return is_64_bit ? 12 : 4; }
        uint32_t SizeofPrologueLength() const { return is_64_bit ? 8 : 4; }
        // Length of the prologue in bytes
        uint32_t Length() const { return prologue_length + SizeofTotalLength() + sizeof(version) + SizeofPrologueLength(); }
        // Length of the line table data in bytes (not including the prologue)
        uint32_t StatementTableLength() const { return total_length + SizeofPrologueLength() - Length(); }
        int32_t MaxLineIncrementForSpecialOpcode() const { return line_base + (int8_t)line_range - 1; }
        bool IsValid() const;
//      void Append(BinaryStreamBuf& buff) const;
        void Dump (lldb_private::Log *log);
        void Clear()
        {
            total_length = version = prologue_length = min_inst_length = line_base = line_range = opcode_base = 0;
            line_base = 0;
            standard_opcode_lengths.clear();
            include_directories.clear();
            file_names.clear();
            is_64_bit = false;
        }
        bool GetFile(uint32_t file_idx, std::string& file, std::string& dir) const;

    };

    // Standard .debug_line state machine structure
    struct Row
    {
        typedef std::vector<Row>            collection;
        typedef collection::iterator        iterator;
        typedef collection::const_iterator  const_iterator;

        Row(bool default_is_stmt = false);
        virtual ~Row() {}
        void PostAppend ();
        void Reset(bool default_is_stmt);
        void Dump(lldb_private::Log *log) const;
        static void Insert(Row::collection& state_coll, const Row& state);
        static void Dump(lldb_private::Log *log, const Row::collection& state_coll);

        dw_addr_t   address;        // The program-counter value corresponding to a machine instruction generated by the compiler.
        uint32_t    line;           // An unsigned integer indicating a source line number. Lines are numbered beginning at 1. The compiler may emit the value 0 in cases where an instruction cannot be attributed to any source line.
        uint16_t    column;         // An unsigned integer indicating a column number within a source line. Columns are numbered beginning at 1. The value 0 is reserved to indicate that a statement begins at the 'left edge' of the line.
        uint16_t    file;           // An unsigned integer indicating the identity of the source file corresponding to a machine instruction.
        uint8_t     is_stmt:1,      // A boolean indicating that the current instruction is the beginning of a statement.
                    basic_block:1,  // A boolean indicating that the current instruction is the beginning of a basic block.
                    end_sequence:1, // A boolean indicating that the current address is that of the first byte after the end of a sequence of target machine instructions.
                    prologue_end:1, // A boolean indicating that the current address is one (of possibly many) where execution should be suspended for an entry breakpoint of a function.
                    epilogue_begin:1;// A boolean indicating that the current address is one (of possibly many) where execution should be suspended for an exit breakpoint of a function.
        uint32_t    isa;            // An unsigned integer whose value encodes the applicable instruction set architecture for the current instruction.
    };


    //------------------------------------------------------------------
    // LineTable
    //------------------------------------------------------------------
    struct LineTable
    {
        typedef std::shared_ptr<LineTable> shared_ptr;

        LineTable() :
            prologue(),
            rows()
        {
        }

        void AppendRow(const DWARFDebugLine::Row& state);
        void Clear()
        {
            prologue.reset();
            rows.clear();
        }

        uint32_t LookupAddress(dw_addr_t address, dw_addr_t cu_high_pc) const;
        void Dump(lldb_private::Log *log) const;

        Prologue::shared_ptr prologue;
        Row::collection rows;
    };

    //------------------------------------------------------------------
    // State
    //------------------------------------------------------------------
    struct State : public Row
    {
        typedef void (*Callback)(dw_offset_t offset, const State& state, void* userData);

        // Special row codes used when calling the callback
        enum
        {
            StartParsingLineTable = 0,
            DoneParsingLineTable = -1
        };

        State (Prologue::shared_ptr& prologue_sp,
               lldb_private::Log *log,
               Callback callback,
               void* userData);

        void
        AppendRowToMatrix (dw_offset_t offset);

        void
        Finalize (dw_offset_t offset);

        void
        Reset ();

        Prologue::shared_ptr prologue;
        lldb_private::Log *log;
        Callback callback; // Callback function that gets called each time an entry is to be added to the matrix
        void* callbackUserData;
        int row; // The row number that starts at zero for the prologue, and increases for each row added to the matrix
    private:
        DISALLOW_COPY_AND_ASSIGN (State);
    };

    static bool DumpOpcodes(lldb_private::Log *log, SymbolFileDWARF* dwarf2Data, dw_offset_t line_offset = DW_INVALID_OFFSET, uint32_t dump_flags = 0);   // If line_offset is invalid, dump everything
    static bool DumpLineTableRows(lldb_private::Log *log, SymbolFileDWARF* dwarf2Data, dw_offset_t line_offset = DW_INVALID_OFFSET);  // If line_offset is invalid, dump everything
    static bool ParseSupportFiles(const lldb::ModuleSP &module_sp, const lldb_private::DWARFDataExtractor& debug_line_data, const char *cu_comp_dir, dw_offset_t stmt_list, lldb_private::FileSpecList &support_files);
    static bool ParsePrologue(const lldb_private::DWARFDataExtractor& debug_line_data, lldb::offset_t* offset_ptr, Prologue* prologue);
    static bool ParseStatementTable(const lldb_private::DWARFDataExtractor& debug_line_data, lldb::offset_t* offset_ptr, State::Callback callback, void* userData);
    static dw_offset_t DumpStatementTable(lldb_private::Log *log, const lldb_private::DWARFDataExtractor& debug_line_data, const dw_offset_t line_offset);
    static dw_offset_t DumpStatementOpcodes(lldb_private::Log *log, const lldb_private::DWARFDataExtractor& debug_line_data, const dw_offset_t line_offset, uint32_t flags);
    static bool ParseStatementTable(const lldb_private::DWARFDataExtractor& debug_line_data, lldb::offset_t *offset_ptr, LineTable* line_table);
    static void Parse(const lldb_private::DWARFDataExtractor& debug_line_data, DWARFDebugLine::State::Callback callback, void* userData);
//  static void AppendLineTableData(const DWARFDebugLine::Prologue* prologue, const DWARFDebugLine::Row::collection& state_coll, const uint32_t addr_size, BinaryStreamBuf &debug_line_data);

    DWARFDebugLine() :
        m_lineTableMap()
    {
    }

    void Parse(const lldb_private::DWARFDataExtractor& debug_line_data);
    void ParseIfNeeded(const lldb_private::DWARFDataExtractor& debug_line_data);
    LineTable::shared_ptr GetLineTable(const dw_offset_t offset) const;

protected:
    typedef std::map<dw_offset_t, LineTable::shared_ptr> LineTableMap;
    typedef LineTableMap::iterator LineTableIter;
    typedef LineTableMap::const_iterator LineTableConstIter;

    LineTableMap m_lineTableMap;
};

#endif  // SymbolFileDWARF_DWARFDebugLine_h_
