#include "dialogconnexion.h"
#include "ui_dialogconnexion.h"

DialogConnexion::DialogConnexion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConnexion)
{
    ui->setupUi(this);
}

DialogConnexion::~DialogConnexion()
{
    delete ui;
}

QString DialogConnexion::getIp()
{
    return ui->lineEditAdresse->text();
}

QString DialogConnexion::getPseudo()
{
    return ui->lineEditPseudo->text();
}

void DialogConnexion::on_pushButtonAnnuler_clicked()
{
    this->done(QDialog::Rejected);
}



void DialogConnexion::on_pushButtonConnecter_clicked()
{
    this->done(QDialog::Accepted);
}
