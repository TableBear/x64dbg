#pragma once

#include <QWidget>
#include "Bridge.h"
#include "StdTable.h"
#include "QZydis.h"

class MenuBuilder;

class BreakpointsView : public StdTable
{
    Q_OBJECT
public:
    explicit BreakpointsView(QWidget* parent = nullptr);

protected:
    void setupContextMenu();
    void updateColors() override;
    void sortRows(duint column, bool ascending) override;
    QString paintContent(QPainter* painter, duint row, duint col, int x, int y, int w, int h) override;

private slots:
    void updateBreakpointsSlot();
    void disassembleAtSlot(duint addr, duint cip);
    void tokenizerConfigUpdatedSlot();
    void contextMenuSlot(const QPoint & pos);
    void followBreakpointSlot();
    void removeBreakpointSlot();
    void toggleBreakpointSlot();
    void editBreakpointSlot();
    void resetHitCountBreakpointSlot();
    void enableAllBreakpointsSlot();
    void disableAllBreakpointsSlot();
    void removeAllBreakpointsSlot();
    void addDllBreakpointSlot();
    void addExceptionBreakpointSlot();
    void copyConditionalBreakpointSlot();
    void pasteConditionalBreakpointSlot();

private:
    enum
    {
        ColType,
        ColAddr,
        ColModLabel,
        ColState,
        ColDisasm,
        ColHits,
        ColSummary
    };

    std::unordered_map<duint, const char*> mExceptionMap;
    QStringList mExceptionList;
    int mExceptionMaxLength;
    std::vector<BRIDGEBP> mBps;
    std::vector<std::pair<RichTextPainter::List, RichTextPainter::List>> mRich;
    QColor mDisasmBackgroundColor;
    QColor mDisasmSelectionColor;
    QColor mCipBackgroundColor;
    QColor mCipColor;
    QColor mSummaryParenColor;
    QColor mSummaryKeywordColor;
    QColor mSummaryStringColor;
    duint mCip = 0;
    MenuBuilder* mMenuBuilder;
    QAction* mEnableDisableAction;
    QZydis* mDisasm;

    const int bpIndex(int i) const
    {
        return mData.at(i).at(ColAddr).userdata;
    }

    const BRIDGEBP & selectedBp(int index = -1) const
    {
        if(index == -1)
            index = getInitialSelection();
        return mBps.at(bpIndex(index));
    }

    bool isValidBp(int sel = -1)
    {
        if(sel == -1)
            sel = getInitialSelection();
        if(!DbgIsDebugging() || mBps.empty() || !isValidIndex(sel, ColType))
            return false;
        auto & bp = mBps.at(bpIndex(sel));
        return bp.addr != 0 || bp.active;
    }

    QString bpTypeName(BPXTYPE type) const
    {
        switch(type)
        {
        case bp_normal:
            return tr("Software");
        case bp_hardware:
            return tr("Hardware");
        case bp_memory:
            return tr("Memory");
        case bp_dll:
            return tr("DLL");
        case bp_exception:
            return tr("Exception");
        default:
            return QString();
        }
    }
};
