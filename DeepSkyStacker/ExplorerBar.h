#ifndef EXPLORERBAR_H
#define EXPLORERBAR_H

#include <QWidget>
#include "mrupath.h"
//#include "ClickLabel.h"

namespace Ui {
class ExplorerBar;
}

class ExplorerBar : public QWidget
{
	Q_OBJECT

typedef QWidget
		Inherited;

public:
    explicit ExplorerBar(QWidget *parent = nullptr);
    ~ExplorerBar();

public slots:
	void onHelp();

protected:
	void mousePressEvent(QMouseEvent*) override;
	void showEvent(QShowEvent* event) override;

private slots:
	void onOpenLights();
	void onOpenDarks();
	void onOpenFlats();
	void onOpenDarkFlats();
	void onOpenBias();

	void onOpenFilelist();
	void onSaveFilelist();
	void onClearList();

	void onCheckAll();
	void onCheckAbove();
	void onUncheckAll();

	void onRegisterChecked();
	void onComputeOffsets();
	void onStackChecked();
	void onBatchStacking();

	void onOpenPicture();
	void onCopyPicture();
	void onDoStarMask();
	void onSavePicture();

	void onSettings();
	void onDDPSettings();
	void onLoadSettings();
	void onSaveSettings();
	void onRecommendedSettings();

    void onAbout();


    //void linkActivated();

private:
    Ui::ExplorerBar *ui;
	bool initialised;
	MRUPath	mruPath;

	void	LoadSettingFile();
	void	SaveSettingFile();

	void	changeEvent(QEvent *);
	void	makeLinks();

	void onInitDialog();
};

#endif // EXPLORERBAR_H
