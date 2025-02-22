/****************************************************************************
**
** Copyright (C) 2020, 2022 David C. Partridge
**
** BSD License Usage
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of DeepSkyStacker nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/
// RegisterSettings.cpp : implementation file
//

#include <algorithm>
using std::min;
using std::max;

#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include <afx.h>
#include <afxcmn.h>
#include <afxcview.h>
#include <afxdlgs.h>

#include <ZExcept.h>
#include <Ztrace.h>

#include <QDialog>
#include <QFileInfo>
#include <QIntValidator>
#include <QSettings>
#include <QShowEvent>
#include <QString>

#include "RegisterSettings.h"
#include "ui/ui_RegisterSettings.h"

extern bool		g_bShowRefStars;

#include "DeepSkyStacker.h"
#include "DSSCommon.h"
#include "commonresource.h"
#include "QtProgressDlg.h"
#include "RegisterEngine.h"
#include "StackingDlg.h"
#include "ProcessingDlg.h"
#include "StackSettings.h"

#include "Workspace.h"

#include "RecommendedSettings.h"

RegisterSettings::RegisterSettings(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::RegisterSettings),
	workspace(new Workspace()),
	initialised(false),
	forceRegister(false),
	stackAfter(false),
	percentStack(0.0),
	noDarks(true),
	noFlats(true),
	noOffsets(true),
	detectionThreshold(0),
	medianFilter(false),
	firstLightFrame(""),
	pStackingTasks(nullptr),
	settingsOnly(false)
{
	ui->setupUi(this);

	perCentValidator = new QIntValidator(0, 100, this);
	ui->percentStack->setValidator(perCentValidator);

	workspace->Push();
}

RegisterSettings::~RegisterSettings()
{
	delete ui;
}

void RegisterSettings::onInitDialog()
{
	QSettings settings;
	QString string;
	bool checked = false;

	//
	// Restore Window position etc..
	//
	QByteArray ba = settings.value("Dialogs/RegisterSettings/geometry").toByteArray();
	if (!ba.isEmpty())
	{
		restoreGeometry(ba);
	}
	else
	{
		//
		// Get main Window rectangle
		//
		const QRect r{ DeepSkyStacker::instance()->rect() };
		QSize size = this->size();

		int top = ((r.top() + (r.height() / 2) - (size.height() / 2)));
		int left = ((r.left() + (r.width() / 2) - (size.width() / 2)));
		move(left, top);
	}
	string = workspace->value("Register/PercentStack", "80").toString();
	ui->percentStack->setText(string);
	percentStack = string.toUInt();

	stackAfter = workspace->value("Register/StackAfter", false).toBool();
	ui->stackAfter->setChecked(stackAfter);
	ui->percentStack->setEnabled(stackAfter);

	ui->hotPixels->setChecked(workspace->value("Register/DetectHotPixels", false).toBool());

	uint value = workspace->value("Register/DetectionThreshold", 10).toUInt();
	ui->luminanceThreshold->
		setSliderPosition(value);
	ui->luminancePercent->setText(QString("%1%").arg(value));

	ui->medianFilter->
		setChecked(workspace->value("Register/ApplyMedianFilter", false).toBool());

	DSS::StackingDlg & stackingDlg = DeepSkyStacker::instance()->getStackingDlg();
	//
	// If there are any stackable light frames, set up the 
	// stacking related stuff
	//
	if (stackingDlg.checkedImageCount(PICTURETYPE_LIGHTFRAME) > 0)
	{
		firstLightFrame = stackingDlg.getFirstCheckedLightFrame();

		forceRegister = !stackingDlg.countUnregisteredCheckedLightFrames();
		noDarks = !stackingDlg.checkedImageCount(PICTURETYPE_DARKFRAME);
		noFlats = !stackingDlg.checkedImageCount(PICTURETYPE_FLATFRAME);;
		noOffsets = !stackingDlg.checkedImageCount(PICTURETYPE_OFFSETFRAME);;
	}

	// Enable the computeDetected Stars button if there's a stackable light frame
	ui->computeDetectedStars->setEnabled(!firstLightFrame.isEmpty());
	if (settingsOnly)
	{
		ui->recommendedSettings->setEnabled(false);
		ui->stackingSettings->setEnabled(false);

		ui->actionsTab->setEnabled(false);
		
		ui->tabWidget->setCurrentWidget(ui->advancedTab);
	}
	else
	{
		if (noDarks || noFlats || noOffsets)
		{
			if (noDarks && noFlats && noOffsets)
				string = tr("Don't forget to add and check dark, flat and offset frames before stacking.",
					"IDS_CHECK_DARKFLATOFFSET");
			else if (noDarks && noFlats)
				string = tr("Don't forget to add and check dark and flat frames before stacking.",
					"IDS_CHECK_DARKFLAT");
			else if (noDarks && noOffsets)
				string = tr("Don't forget to add and check dark and offset frames before stacking.",
					"IDS_CHECK_DARKOFFSET");
			else if (noFlats && noOffsets)
				string = tr("Don't forget to add and check flat and offset frames before stacking.",
					"IDS_CHECK_FLATOFFSET");
			else if (noDarks)
				string = tr("Don't forget to add and check dark frames before stacking.",
					"IDS_CHECK_DARK");
			else if (noFlats)
				string = tr("Don't forget to add and check flat frames before stacking.",
					"IDS_CHECK_FLAT");
			else if (noOffsets)
				string = tr("Don't forget to add and check offset frames before stacking.",
					"IDS_CHECK_OFFSET");

			switch (noDarks + noFlats + noOffsets)
			{
			case 3:
				ui->stackWarning->setStyleSheet(
					"QLabel { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
					"stop:0 rgb(252, 220, 221), stop:1 rgb(255, 64, 64)) }" );
				break;
			case 2:
				ui->stackWarning->setStyleSheet(
					"QLabel { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
					"stop:0 rgb(252, 220, 221), stop:1 rgb(255, 171, 63)) }" );
				break;
			case 1:
				ui->stackWarning->setStyleSheet(
					"QLabel { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
					"stop:0 rgb(252, 220, 221), stop:1 rgb(255, 234, 63)) }" );
				break;
			};
		}
		else
		{
			string = tr("darks, flats and offsets/bias checked.",
				"IDS_CHECK_ALLOK");
			ui->stackWarning->setStyleSheet(
				"QLabel { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
				"stop:0 rgb(229, 255, 193), stop:1 rgb(21, 223, 33)) }" );
		};
		ui->stackWarning->setText(string);

	}

	ui->forceRegister->setChecked(forceRegister);
}

void RegisterSettings::on_forceRegister_stateChanged(int state)
{
	state;
	//
	// Only used by StackingDlg.cpp by invoking isForceRegister() after 
	// calling exec() on this dialog.  So only held as a class variable
	//
	forceRegister = ui->forceRegister->isChecked();
}

void RegisterSettings::on_hotPixels_stateChanged(int state)
{
	state;
	bool hotPixels = ui->hotPixels->isChecked();
	workspace->setValue("Register/DetectHotPixels", hotPixels);
}

void RegisterSettings::on_stackAfter_clicked()
{
	stackAfter = ui->stackAfter->isChecked();
	workspace->setValue("Register/StackAfter", stackAfter);
}

void RegisterSettings::on_percentStack_textEdited(const QString &text)
{
	percentStack = text.toUInt();
	workspace->setValue("Register/PercentStack", text);
}

void RegisterSettings::on_luminanceThreshold_valueChanged(int newValue)
{
	if (detectionThreshold != newValue)
	{
		detectionThreshold = newValue;
		// Display new value
		ui->luminancePercent->setText(QString("%1%").arg(newValue, 3));
	}
}

void RegisterSettings::on_computeDetectedStars_clicked()
{
	// Retrieve the first checked light frame of the list
	DSS::DSSProgressDlg				dlg;
	CLightFrameInfo				fi;

	QFileInfo info(firstLightFrame);
	QString fileName = info.fileName();

	QString string = tr("Registering %1", "IDS_REGISTERINGNAME").arg(fileName);

	dlg.Start(CString((wchar_t*)string.utf16()), 0, false);
	dlg.SetJointProgress(true);
	fi.RegisterPicture(CString(reinterpret_cast<const wchar_t*>(firstLightFrame.utf16())), static_cast<double>(detectionThreshold) / 100.0, true, medianFilter, &dlg);
	dlg.SetJointProgress(false);

	string = tr("%1 star(s)", "IDC_NRSTARS").arg(fi.m_vStars.size());
	ui->starCount->setText(string);
}

void RegisterSettings::on_medianFilter_stateChanged(int state)
{
	state;
	medianFilter = ui->medianFilter->isChecked();
	workspace->setValue("Register/ApplyMedianFilter", medianFilter);
} 

void RegisterSettings::on_recommendedSettings_clicked()
{
	RecommendedSettings		dlg;

	dlg.setStackingTasks(pStackingTasks);

	if (dlg.exec())
	{
		if (pStackingTasks)
			pStackingTasks->UpdateTasksMethods();

		ui->medianFilter->setChecked(workspace->value("Register/ApplyMedianFilter").toBool());
	};
}

void RegisterSettings::on_stackingSettings_clicked()
{
	StackSettings dlg(this);

	QRect	rcCustom;

	if (pStackingTasks)
	{
		if (pStackingTasks->GetCustomRectangle(rcCustom))
		{
			dlg.enableCustomRectangle(true);
			dlg.selectCustomRectangle(pStackingTasks->IsCustomRectangleUsed());
		}
		else
		{
			dlg.enableCustomRectangle(false);
			dlg.selectCustomRectangle(false);
		}
		dlg.setTabVisibility(pStackingTasks->AreDarkUsed(),
			pStackingTasks->AreFlatUsed(),
			pStackingTasks->AreBiasUsed());
	}
	else
	{
		dlg.enableCustomRectangle(false);
		dlg.selectCustomRectangle(false);
	}

	if (!ui->stackAfter->isChecked())
		dlg.setRegisteringOnly(true);

	dlg.setStackingTasks(pStackingTasks);

	if (dlg.exec() && pStackingTasks)
		pStackingTasks->UpdateTasksMethods();
}

void RegisterSettings::accept()
{
	QSettings settings;

	settings.setValue("Dialogs/RegisterSettings/geometry", saveGeometry());

	// Save the luminance detection threshold which wasn't saved in 
	// the valueChanged() slot
	workspace->setValue("Register/DetectionThreshold", detectionThreshold);
	//
	// Pop the preserved workspace setting and discard the saved values
	//
	workspace->Pop(false);

	//
	// Harden the workspace changes
	//
	workspace->saveSettings();

	Inherited::accept();
}

void RegisterSettings::reject()
{
	QSettings settings;

	settings.setValue("Dialogs/RegisterSettings/geometry", saveGeometry());

	//
	// Pop the preserved workspace setting and restore the status quo ante 
	//
	workspace->Pop(true);

	Inherited::reject();

}

void RegisterSettings::showEvent(QShowEvent *event)
{
	if (!event->spontaneous())
	{
		if (!initialised)
		{
			initialised = true;
			onInitDialog();
		}
	}
	// Invoke base class showEvent()
	return Inherited::showEvent(event);
}
