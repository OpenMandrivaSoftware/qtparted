/*
	qtparted - a frontend to libparted for manipulating disk partitions
	Copyright (C) 2002-2003 Vanni Brutto

	Vanni Brutto <zanac (-at-) libero dot it>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <qcombobox.h>
#include <qlineedit.h>
#include "qp_dlgconfig.h"
#include "qp_common.h"

QP_dlgConfig::QP_dlgConfig(QWidget *p):QDialog(p),_layout(this) {
	setWindowTitle(tr("Configuration"));

	_lblLayout = new QLabel(tr("QTParted &layout"), this);
	_layout.addWidget(_lblLayout);

	_cmbLayout = new QComboBox(this);
	_lblLayout->setBuddy(_cmbLayout);
	_cmbLayout->clear();
	_cmbLayout->insertItems(0, QStringList()
		<< tr("Chart and ListBox")
		<< tr("Only Chart")
		<< tr("Only ListBox")
	);
	_layout.addWidget(_cmbLayout);

	_lblExtTools = new QLabel(tr("&External tools"), this);
	_layout.addWidget(_lblExtTools);

	_cmbExtTools = new QComboBox(this);
	_layout.addWidget(_cmbExtTools);
	_lblExtTools->setBuddy(_cmbExtTools);

	_lblPath = new QLabel("&Full executable path", this);
	_layout.addWidget(_lblPath);

	_txtPath = new QLineEdit(this);
	_lblPath->setBuddy(_txtPath);
	_layout.addWidget(_txtPath);

	_buttonLayout = new QHBoxLayout();
	_btnOk = new QPushButton(tr("&OK"), this);
	_buttonLayout->addWidget(_btnOk);
	connect(_btnOk, SIGNAL(clicked()), this, SLOT(accept()));

	_btnCancel = new QPushButton(tr("&Cancel"), this);
	_buttonLayout->addWidget(_btnCancel);
	connect(_btnCancel, SIGNAL(clicked()), this, SLOT(reject()));

	_layout.addLayout(_buttonLayout);
	
	/*---clear combo box used for external tools---*/
	_cmbExtTools->clear();
	for(QP_ListExternalTools::ConstIterator it = lstExternalTools->begin(); it != lstExternalTools->end(); ++it)
		_cmbExtTools->addItem(it.value()->name());

	/*---connect the combo type slot---*/
	connect(_cmbExtTools, SIGNAL(activated(int)),
			this, SLOT(slotToolChanged(int)));

	/*---connect the path text changed slot---*/
	connect(_txtPath, SIGNAL(textChanged(const QString &)),
			this, SLOT(slotPathChanged(const QString &)));
}

QP_dlgConfig::~QP_dlgConfig() {
}

void QP_dlgConfig::init_dialog() {
	slotToolChanged(0);
}

int QP_dlgConfig::show_dialog() {
	return exec();
}

int QP_dlgConfig::layout() {
	return _cmbLayout->currentIndex();
}

void QP_dlgConfig::setLayout(int layout) {
	_cmbLayout->setCurrentIndex(layout);
}

void QP_dlgConfig::slotToolChanged(int) {
	QString path = lstExternalTools->getPath(_cmbExtTools->currentText());
	_txtPath->setText(path);

	QString tooltip = lstExternalTools->getDescription(_cmbExtTools->currentText());
	_txtPath->setWhatsThis(tooltip);
}

void QP_dlgConfig::slotPathChanged(const QString &path) {
	lstExternalTools->setPath(_cmbExtTools->currentText(), path);
}
