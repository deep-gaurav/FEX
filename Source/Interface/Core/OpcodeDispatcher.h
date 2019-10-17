#pragma once

#include <FEXCore/IR/IntrusiveIRList.h>

#include <FEXCore/Debug/X86Tables.h>
#include <FEXCore/IR/IR.h>

#include <cstdint>
#include <functional>
#include <map>

namespace FEXCore::IR {
class Pass;
class PassManager;

class OpDispatchBuilder final {
friend class FEXCore::IR::Pass;
friend class FEXCore::IR::PassManager;
public:

  struct {
    bool HadUnconditionalExit {false};
  } Information;
  bool ShouldDump {false};

  struct Fixup {
    OrderedNode *Node;
    IROp_Header *Op;
  };

  std::map<uint64_t, std::vector<Fixup>> Fixups;
  std::map<uint64_t, OrderedNode *> JumpTargets;

  OpDispatchBuilder();

  IRListView<false> ViewIR() { return IRListView<false>(&Data, &ListData); }
  IRListView<true> *CreateIRCopy() { return new IRListView<true>(&Data, &ListData); }
  void ResetWorkingList();
  bool HadDecodeFailure() { return DecodeFailure; }

  void BeginFunction(uint64_t RIP);
  void ExitFunction();
  void Finalize();

  // Dispatch builder functions
#define OpcodeArgs [[maybe_unused]] FEXCore::X86Tables::DecodedOp Op
  void UnhandledOp(OpcodeArgs);
  void MOVOp(OpcodeArgs);
  void ALUOp(OpcodeArgs);
  void INTOp(OpcodeArgs);
  void SyscallOp(OpcodeArgs);
  void LEAOp(OpcodeArgs);
  void NOPOp(OpcodeArgs);
  void RETOp(OpcodeArgs);
  void SecondaryALUOp(OpcodeArgs);
  void ADCOp(OpcodeArgs);
  void SBBOp(OpcodeArgs);
  void PUSHOp(OpcodeArgs);
  void POPOp(OpcodeArgs);
  void LEAVEOp(OpcodeArgs);
  void CALLOp(OpcodeArgs);
  void CALLAbsoluteOp(OpcodeArgs);
  void CondJUMPOp(OpcodeArgs);
  void JUMPOp(OpcodeArgs);
  void JUMPAbsoluteOp(OpcodeArgs);
  void TESTOp(OpcodeArgs);
  void MOVSXDOp(OpcodeArgs);
  void MOVSXOp(OpcodeArgs);
  void MOVZXOp(OpcodeArgs);
  void CMPOp(OpcodeArgs);
  void SETccOp(OpcodeArgs);
  void CQOOp(OpcodeArgs);
  void CDQOp(OpcodeArgs);
  void XCHGOp(OpcodeArgs);
  void SAHFOp(OpcodeArgs);
  void LAHFOp(OpcodeArgs);
  void MOVSegOp(OpcodeArgs);
  void FLAGControlOp(OpcodeArgs);
  void MOVOffsetOp(OpcodeArgs);
  void CMOVOp(OpcodeArgs);
  void CPUIDOp(OpcodeArgs);
  void SHLOp(OpcodeArgs);
  template<bool SHR1Bit>
  void SHROp(OpcodeArgs);
  void ASHROp(OpcodeArgs);
  void ROROp(OpcodeArgs);
  void ROLOp(OpcodeArgs);
  void BTOp(OpcodeArgs);
  void IMUL1SrcOp(OpcodeArgs);
  void IMUL2SrcOp(OpcodeArgs);
  void IMULOp(OpcodeArgs);
  void STOSOp(OpcodeArgs);
  void MOVSOp(OpcodeArgs);
  void CMPSOp(OpcodeArgs);
  void BSWAPOp(OpcodeArgs);

  void RDTSCOp(OpcodeArgs);
  void INCOp(OpcodeArgs);
  void DECOp(OpcodeArgs);
  void NEGOp(OpcodeArgs);
  void DIVOp(OpcodeArgs);
  void IDIVOp(OpcodeArgs);
  void BSFOp(OpcodeArgs);
  void BSROp(OpcodeArgs);
  void CMPXCHGOp(OpcodeArgs);
  void MULOp(OpcodeArgs);
  void NOTOp(OpcodeArgs);

  // SSE
  void MOVUPSOp(OpcodeArgs);
  void MOVLHPSOp(OpcodeArgs);
  void MOVHPDOp(OpcodeArgs);
  void VectorALUOp(OpcodeArgs);
  void MOVQOp(OpcodeArgs);
  void PADDQOp(OpcodeArgs);
  void PSUBQOp(OpcodeArgs);
  template<size_t ElementSize>
  void PMINUOp(OpcodeArgs);
  void PMINSWOp(OpcodeArgs);
  void PMOVMSKBOp(OpcodeArgs);
  void PUNPCKLOp(OpcodeArgs);
  void PUNPCKHOp(OpcodeArgs);
  template<size_t ElementSize, bool Low>
  void PSHUFDOp(OpcodeArgs);
  void PCMPEQOp(OpcodeArgs);
  template<size_t ElementSize>
  void PCMPGTOp(OpcodeArgs);
  void MOVDOp(OpcodeArgs);
  template<size_t ElementSize>
  void PSRLD(OpcodeArgs);
  template<size_t ElementSize, bool Scalar>
  void PSLL(OpcodeArgs);
  void PSRLDQ(OpcodeArgs);
  void MOVDDUPOp(OpcodeArgs);

  template<size_t ElementSize>
  void SHUFOp(OpcodeArgs);

  void FXSaveOp(OpcodeArgs);
  void FXRStoreOp(OpcodeArgs);

  void PAlignrOp(OpcodeArgs);
  void UnimplementedOp(OpcodeArgs);

#undef OpcodeArgs

  /**
   * @name IR allocation routines
   *
   * @{ */

// These handlers add cost to the constructor and destructor
// If it becomes an issue then blow them away
// GCC also generates some pretty atrocious code around these
// Use Clang!
#define IROP_ALLOCATE_HELPERS
#define IROP_DISPATCH_HELPERS
#include "IRDefines.inc"

  IRPair<IROp_Constant> _Constant(uint8_t Size, uint64_t Constant) {
    auto Op = AllocateOp<IROp_Constant, IROps::OP_CONSTANT>();
    Op.first->Constant = Constant;
    Op.first->Header.Size = Size / 8;
    Op.first->Header.Elements = 1;
    Op.first->Header.NumArgs = 0;
    Op.first->Header.HasDest = true;
    return Op;
  }

	IRPair<IROp_Bfe> _Bfe(uint8_t Width, uint8_t lsb, OrderedNode *ssa0) {
    return _Bfe(ssa0, Width, lsb);
  }
	IRPair<IROp_Bfi> _Bfi(uint8_t Width, uint8_t lsb, OrderedNode *ssa0, OrderedNode *ssa1) {
    return _Bfi(ssa0, ssa1, Width, lsb);
  }
  IRPair<IROp_StoreMem> _StoreMem(uint8_t Size, OrderedNode *ssa0, OrderedNode *ssa1) {
    return _StoreMem(ssa0, ssa1, Size);
  }
	IRPair<IROp_LoadMem> _LoadMem(uint8_t Size, OrderedNode *ssa0) {
    return _LoadMem(ssa0, Size);
  }
	IRPair<IROp_StoreContext> _StoreContext(uint8_t Size, uint32_t Offset, OrderedNode *ssa0) {
    return _StoreContext(ssa0, Size, Offset);
  }
	IRPair<IROp_Select> _Select(uint8_t Cond, OrderedNode *ssa0, OrderedNode *ssa1, OrderedNode *ssa2, OrderedNode *ssa3) {
    return _Select(ssa0, ssa1, ssa2, ssa3, {Cond});
  }
	IRPair<IROp_Sext> _Sext(uint8_t SrcSize, OrderedNode *ssa0) {
    return _Sext(ssa0, SrcSize);
  }
	IRPair<IROp_Zext> _Zext(uint8_t SrcSize, OrderedNode *ssa0) {
    return _Zext(ssa0, SrcSize);
  }
	IRPair<IROp_VInsElement> _VInsElement(uint8_t RegisterSize, uint8_t ElementSize, uint8_t DestIdx, uint8_t SrcIdx, OrderedNode *ssa0, OrderedNode *ssa1) {
    return _VInsElement(ssa0, ssa1, RegisterSize, ElementSize, DestIdx, SrcIdx);
  }
	IRPair<IROp_VAdd> _VAdd(uint8_t RegisterSize, uint8_t ElementSize, OrderedNode *ssa0, OrderedNode *ssa1) {
    return _VAdd(ssa0, ssa1, RegisterSize, ElementSize);
  }
	IRPair<IROp_VSub> _VSub(uint8_t RegisterSize, uint8_t ElementSize, OrderedNode *ssa0, OrderedNode *ssa1) {
    return _VSub(ssa0, ssa1, RegisterSize, ElementSize);
  }
  IRPair<IROp_VUMin> _VUMin(uint8_t RegisterSize, uint8_t ElementSize, OrderedNode *ssa0, OrderedNode *ssa1) {
    return _VUMin(ssa0, ssa1, RegisterSize, ElementSize);
  }
	IRPair<IROp_VSMin> _VSMin(uint8_t RegisterSize, uint8_t ElementSize, OrderedNode *ssa0, OrderedNode *ssa1) {
    return _VSMin(ssa0, ssa1, RegisterSize, ElementSize);
  }
	IRPair<IROp_VZip> _VZip(uint8_t RegisterSize, uint8_t ElementSize, OrderedNode *ssa0, OrderedNode *ssa1) {
    return _VZip(ssa0, ssa1, RegisterSize, ElementSize);
  }
	IRPair<IROp_VZip2> _VZip2(uint8_t RegisterSize, uint8_t ElementSize, OrderedNode *ssa0, OrderedNode *ssa1) {
    return _VZip2(ssa0, ssa1, RegisterSize, ElementSize);
  }
	IRPair<IROp_VCMPEQ> _VCMPEQ(uint8_t RegisterSize, uint8_t ElementSize, OrderedNode *ssa0, OrderedNode *ssa1) {
    return _VCMPEQ(ssa0, ssa1, RegisterSize, ElementSize);
  }
	IRPair<IROp_VCMPGT> _VCMPGT(uint8_t RegisterSize, uint8_t ElementSize, OrderedNode *ssa0, OrderedNode *ssa1) {
    return _VCMPGT(ssa0, ssa1, RegisterSize, ElementSize);
  }

	IRPair<IROp_VUShl> _VUShl(uint8_t RegisterSize, uint8_t ElementSize, OrderedNode *ssa0, OrderedNode *ssa1) {
    return _VUShl(ssa0, ssa1, RegisterSize, ElementSize);
  }
	IRPair<IROp_VUShlS> _VUShlS(uint8_t RegisterSize, uint8_t ElementSize, OrderedNode *ssa0, OrderedNode *ssa1) {
    return _VUShlS(ssa0, ssa1, RegisterSize, ElementSize);
  }

	IRPair<IROp_VUShr> _VUShr(uint8_t RegisterSize, uint8_t ElementSize, OrderedNode *ssa0, OrderedNode *ssa1) {
    return _VUShr(ssa0, ssa1, RegisterSize, ElementSize);
  }
  IRPair<IROp_VExtr> _VExtr(uint8_t RegisterSize, uint8_t ElementSize, OrderedNode *ssa0, OrderedNode *ssa1, uint8_t Index) {
    return _VExtr(ssa0, ssa1, RegisterSize, ElementSize, Index);
  }
  IRPair<IROp_Jump> _Jump() {
    return _Jump(InvalidNode);
  }
  IRPair<IROp_CondJump> _CondJump(OrderedNode *ssa0) {
    return _CondJump(ssa0, InvalidNode);
  }

  void SetJumpTarget(IR::IROp_Jump *Op, OrderedNode *Target) {
    LogMan::Throw::A(Target->Op(Data.Begin())->Op == OP_CODEBLOCK,
        "Tried setting Jump target to %%ssa%d %s",
        Target->Wrapped(ListData.Begin()).ID(),
        std::string(IR::GetName(Target->Op(Data.Begin())->Op)).c_str());

    Op->Header.Args[0].NodeOffset = Target->Wrapped(ListData.Begin()).NodeOffset;
  }
  void SetJumpTarget(IR::IROp_CondJump *Op, OrderedNode *Target) {
    LogMan::Throw::A(Target->Op(Data.Begin())->Op == OP_CODEBLOCK,
        "Tried setting CondJump target to %%ssa%d %s",
        Target->Wrapped(ListData.Begin()).ID(),
        std::string(IR::GetName(Target->Op(Data.Begin())->Op)).c_str());

    Op->Header.Args[1].NodeOffset = Target->Wrapped(ListData.Begin()).NodeOffset;
  }

  void SetJumpTarget(IRPair<IROp_Jump> Op, OrderedNode *Target) {
    LogMan::Throw::A(Target->Op(Data.Begin())->Op == OP_CODEBLOCK,
        "Tried setting Jump target to %%ssa%d %s",
        Target->Wrapped(ListData.Begin()).ID(),
        std::string(IR::GetName(Target->Op(Data.Begin())->Op)).c_str());

    Op.first->Header.Args[0].NodeOffset = Target->Wrapped(ListData.Begin()).NodeOffset;
  }
  void SetJumpTarget(IRPair<IROp_CondJump> Op, OrderedNode *Target) {
    LogMan::Throw::A(Target->Op(Data.Begin())->Op == OP_CODEBLOCK,
        "Tried setting CondJump target to %%ssa%d %s",
        Target->Wrapped(ListData.Begin()).ID(),
        std::string(IR::GetName(Target->Op(Data.Begin())->Op)).c_str());
    Op.first->Header.Args[1].NodeOffset = Target->Wrapped(ListData.Begin()).NodeOffset;
  }

  /**  @} */

  OrderedNode *InsertJumpTarget(uint64_t RIP, OrderedNode *Node) {
    LogMan::Throw::A(JumpTargets.find(RIP) == JumpTargets.end(), "Trying to insert JumpTarget that already exists!");
    LogMan::Throw::A(Node->Op(Data.Begin())->Op == OP_CODEBLOCK,
      "Tried inserting an invalid jump target of type %s",
      std::string(IR::GetName(Node->Op(Data.Begin())->Op)).c_str());
    JumpTargets[RIP] = Node;
    return Node;
  }

  OrderedNode *GetJumpTargetIfExists(uint64_t RIP) {
    auto Node = JumpTargets.find(RIP);
    if (Node == JumpTargets.end()) return nullptr;
    return Node->second;
  }

  bool IsValueConstant(OrderedNodeWrapper ssa, uint64_t *Constant) {
     OrderedNode *RealNode = ssa.GetNode(ListData.Begin());
     FEXCore::IR::IROp_Header *IROp = RealNode->Op(Data.Begin());
     if (IROp->Op == OP_CONSTANT) {
       auto Op = IROp->C<IR::IROp_Constant>();
       *Constant = Op->Constant;
       return true;
     }
     return false;
  }

  // This is fairly special in how it operates
  // Since the node is decoupled from the backing op then we can swap out the backing op without much overhead
  // This can potentially cause problems where multiple nodes are pointing to the same IROp
  OrderedNode *ReplaceAllUsesWith(OrderedNode *Node, IROp_Header *Op) {
    Node->Header.Value.SetOffset(Data.Begin(), reinterpret_cast<uintptr_t>(Op));
    return Node;
  }

  // This is similar to the previous op except that we pass in a node
  // This takes the op backing in the new node and replaces the node in the other node
  // Again can cause problems where things are pointing to NewNode and haven't been decoupled
  OrderedNode *ReplaceAllUsesWith(OrderedNode *Node, OrderedNode *NewNode) {
    Node->Header.Value.NodeOffset = NewNode->Header.Value.NodeOffset;
    return Node;
  }

  void ReplaceAllUsesWithInclusive(OrderedNode *Node, OrderedNode *NewNode, IR::NodeWrapperIterator After, IR::NodeWrapperIterator End);

  void Unlink(OrderedNode *Node) {
    Node->Unlink(ListData.Begin());
  }

  void SetPackedRFLAG(bool Lower8, OrderedNode *Src);
  OrderedNode *GetPackedRFLAG(bool Lower8);

  void CopyData(OpDispatchBuilder const &rhs) {
    LogMan::Throw::A(rhs.Data.BackingSize() <= Data.BackingSize(), "Trying to take ownership of data that is too large");
    LogMan::Throw::A(rhs.ListData.BackingSize() <= ListData.BackingSize(), "Trying to take ownership of data that is too large");
    Data.CopyData(rhs.Data);
    ListData.CopyData(rhs.ListData);
    InvalidNode = rhs.InvalidNode;
    CurrentWriteCursor = rhs.CurrentWriteCursor;
    CodeBlocks = rhs.CodeBlocks;
  }

  void SetWriteCursor(OrderedNode *Node) {
    CurrentWriteCursor = Node;
  }

  OrderedNode *GetWriteCursor() {
    return CurrentWriteCursor;
  }

  /**
   * @brief This creates an orphaned code node
   * The IROp backing is in the correct list but the OrderedNode lives outside of the list
   *
   * XXX: This is because we don't want code blocks to interleave with current instruction IR ops currently
   * We can change this behaviour once we remove the old BeginBlock/EndBlock types
   *
   * @return OrderedNode
   */
  IRPair<IROp_CodeBlock> CreateCodeNode() {
    auto CodeNode = _CodeBlock(InvalidNode, InvalidNode, InvalidNode);
    CodeBlocks.emplace_back(CodeNode);
    return CodeNode;
  }

  void SetCodeNodeBegin(OrderedNode *CodeNode, OrderedNode *Begin) {
     FEXCore::IR::IROp_CodeBlock *IROp = CodeNode->Op(Data.Begin())->CW<FEXCore::IR::IROp_CodeBlock>();
     LogMan::Throw::A(IROp->Header.Op == IROps::OP_CODEBLOCK, "Invalid");
     IROp->Begin = Begin->Wrapped(ListData.Begin());
  }

  void SetCodeNodeLast(OrderedNode *CodeNode, OrderedNode *Last) {
     FEXCore::IR::IROp_CodeBlock *IROp = CodeNode->Op(Data.Begin())->CW<FEXCore::IR::IROp_CodeBlock>();
     LogMan::Throw::A(IROp->Header.Op == IROps::OP_CODEBLOCK, "Invalid");
     IROp->Last = Last->Wrapped(ListData.Begin());
  }

  void LinkCodeBlocks(OrderedNode *CodeNode, OrderedNode *Next) {
     FEXCore::IR::IROp_CodeBlock *IROp = CodeNode->Op(Data.Begin())->CW<FEXCore::IR::IROp_CodeBlock>();
     LogMan::Throw::A(IROp->Header.Op == IROps::OP_CODEBLOCK, "Invalid");
     IROp->Next = Next->Wrapped(ListData.Begin());
  }

  IRPair<IROp_CodeBlock> CreateNewBeginBlock();
  IRPair<IROp_EndBlock> CreateNewEndBlock(uint64_t RIPIncrement);

  void SetMultiblock(bool _Multiblock) { Multiblock = _Multiblock; }
  bool GetMultiblock() { return Multiblock; }

private:
  void TestFunction();
  bool DecodeFailure{false};

  OrderedNode *LoadSource(FEXCore::X86Tables::DecodedOp const& Op, FEXCore::X86Tables::DecodedOperand const& Operand, uint32_t Flags, bool LoadData = true, bool ForceLoad = false);
  OrderedNode *LoadSource_WithOpSize(FEXCore::X86Tables::DecodedOp const& Op, FEXCore::X86Tables::DecodedOperand const& Operand, uint8_t OpSize, uint32_t Flags, bool LoadData = true, bool ForceLoad = false);
  void StoreResult_WithOpSize(FEXCore::X86Tables::DecodedOp Op, FEXCore::X86Tables::DecodedOperand const& Operand, OrderedNode *const Src, uint8_t OpSize);
  void StoreResult(FEXCore::X86Tables::DecodedOp Op, FEXCore::X86Tables::DecodedOperand const& Operand, OrderedNode *const Src);
  void StoreResult(FEXCore::X86Tables::DecodedOp Op, OrderedNode *const Src);
  uint8_t GetDstSize(FEXCore::X86Tables::DecodedOp Op);
  uint8_t GetSrcSize(FEXCore::X86Tables::DecodedOp Op);

  template<unsigned BitOffset>
  void SetRFLAG(OrderedNode *Value);
  void SetRFLAG(OrderedNode *Value, unsigned BitOffset);
  OrderedNode *GetRFLAG(unsigned BitOffset);

  void GenerateFlags_ADC(FEXCore::X86Tables::DecodedOp Op, OrderedNode *Res, OrderedNode *Src1, OrderedNode *Src2, OrderedNode *CF);
  void GenerateFlags_SBB(FEXCore::X86Tables::DecodedOp Op, OrderedNode *Res, OrderedNode *Src1, OrderedNode *Src2, OrderedNode *CF);
  void GenerateFlags_SUB(FEXCore::X86Tables::DecodedOp Op, OrderedNode *Res, OrderedNode *Src1, OrderedNode *Src2);
  void GenerateFlags_ADD(FEXCore::X86Tables::DecodedOp Op, OrderedNode *Res, OrderedNode *Src1, OrderedNode *Src2);
  void GenerateFlags_MUL(FEXCore::X86Tables::DecodedOp Op, OrderedNode *Res, OrderedNode *High);
  void GenerateFlags_UMUL(FEXCore::X86Tables::DecodedOp Op, OrderedNode *High);
  void GenerateFlags_Logical(FEXCore::X86Tables::DecodedOp Op, OrderedNode *Res, OrderedNode *Src1, OrderedNode *Src2);
  void GenerateFlags_Shift(FEXCore::X86Tables::DecodedOp Op, OrderedNode *Res, OrderedNode *Src1, OrderedNode *Src2);
  void GenerateFlags_Rotate(FEXCore::X86Tables::DecodedOp Op, OrderedNode *Res, OrderedNode *Src1, OrderedNode *Src2);

  OrderedNode *CreateNode(IROp_Header *Op) {
    uintptr_t ListBegin = ListData.Begin();
    size_t Size = sizeof(OrderedNode);
    void *Ptr = ListData.Allocate(Size);
    OrderedNode *Node = new (Ptr) OrderedNode();
    Node->Header.Value.SetOffset(Data.Begin(), reinterpret_cast<uintptr_t>(Op));

    if (CurrentWriteCursor) {
      CurrentWriteCursor->append(ListBegin, Node);
    }
    CurrentWriteCursor = Node;
    return Node;
  }

  OrderedNode *GetNode(uint32_t SSANode) {
    uintptr_t ListBegin = ListData.Begin();
    OrderedNode *Node = reinterpret_cast<OrderedNode *>(ListBegin + SSANode * sizeof(OrderedNode));
    return Node;
  }

  OrderedNode *EmplaceOrphanedNode(OrderedNode *OldNode) {
    size_t Size = sizeof(OrderedNode);
    OrderedNode *Ptr = reinterpret_cast<OrderedNode*>(ListData.Allocate(Size));
    memcpy(Ptr, OldNode, Size);
    return Ptr;
  }

  OrderedNode *CurrentWriteCursor = nullptr;

  // These could be combined with a little bit of work to be more efficient with memory usage. Isn't a big deal
  IntrusiveAllocator Data;
  IntrusiveAllocator ListData;

  OrderedNode *InvalidNode;
  OrderedNode *CurrentCodeBlock{};
  std::vector<OrderedNode*> CodeBlocks;
  bool Multiblock{};
  uint64_t Entry;
};

void InstallOpcodeHandlers();

}

