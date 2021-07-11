#ifndef GHIDRADECVIEW_H
#define GHIDRADECVIEW_H

#include <QWidget>
#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <bridgemain.h>

struct GhidraDecRange
{
    duint start;
    duint end;
};

class GhidraDecView : public QWidget
{
    Q_OBJECT
public:
    explicit GhidraDecView(QWidget* parent = nullptr);
    void decompileAt(const GhidraDecRange* ranges, duint count) const;

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void populateInstructionsContextMenu(QMenu* menu) const;
    void populateCxxContextMenu(QMenu* menu) const;
    void jumpFromInstructionsView() const;
    void jumpFromCxxView() const;

private:
    QMainWindow* mGhidraDecMainWindow;
    QAction* mJumpFromInstructionsViewAction;
    QAction* mJumpFromCxxViewAction;
};

#endif // GHIDRADECVIEW_H