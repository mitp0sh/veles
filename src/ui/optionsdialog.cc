/*
 * Copyright 2016 CodiLime
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "include/ui/optionsdialog.h"
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include "ui_optionsdialog.h"
#include "util/settings/hexedit.h"
#include "util/settings/theme.h"

namespace veles {
namespace ui {

OptionsDialog::OptionsDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::OptionsDialog) {
  ui->setupUi(this);
  ui->colorsBox->addItems(util::settings::theme::availableThemes());

  color_dialog = new QColorDialog(this);
  color_dialog->setCurrentColor(util::settings::hexedit::colorOfText());
  ui->colorHexEdit->setAutoFillBackground(true);
  ui->colorHexEdit->setFlat(true);

  connect(ui->colorHexEdit, &QPushButton::clicked, color_dialog,
          &QColorDialog::show);

  connect(color_dialog, &QColorDialog::colorSelected, this,
          &OptionsDialog::updateColorButton);

  connect(ui->hexColumnsAutoCheckBox, &QCheckBox::stateChanged,
          [this](int state) {
            ui->hexColumnsSpinBox->setEnabled(state != Qt::Checked);
          });
}

OptionsDialog::~OptionsDialog() {
  color_dialog->deleteLater();
  delete ui;
}

void OptionsDialog::updateColorButton() {
  QPalette pal = ui->colorHexEdit->palette();
  pal.setColor(QPalette::Button, color_dialog->currentColor());
  ui->colorHexEdit->setPalette(pal);
  ui->colorHexEdit->update();
}

void OptionsDialog::show() {
  ui->colorsBox->setCurrentText(util::settings::theme::currentTheme());
  Qt::CheckState checkState = Qt::Unchecked;
  if (util::settings::hexedit::resizeColumnsToWindowWidth()) {
    checkState = Qt::Checked;
  }
  ui->hexColumnsAutoCheckBox->setCheckState(checkState);
  ui->hexColumnsSpinBox->setValue(util::settings::hexedit::columnsNumber());
  ui->hexColumnsSpinBox->setEnabled(checkState != Qt::Checked);
  updateColorButton();

  QWidget::show();
}

void OptionsDialog::accept() {
  bool restart_needed = false;
  QString newTheme = ui->colorsBox->currentText();
  if (newTheme != util::settings::theme::currentTheme()) {
    veles::util::settings::theme::setCurrentTheme(newTheme);
    restart_needed = true;
  }

  util::settings::hexedit::setResizeColumnsToWindowWidth(
      ui->hexColumnsAutoCheckBox->checkState() == Qt::Checked);
  util::settings::hexedit::setColumnsNumber(ui->hexColumnsSpinBox->value());
  util::settings::hexedit::setColorOfText(color_dialog->currentColor());

  if (restart_needed) {
    QMessageBox::about(
        this, tr("Options change"),
        tr("Some changes will only take effect after application restart"));
  }

  emit accepted();
  QDialog::hide();
}

}  // namespace ui
}  // namespace veles
