#include "dialog.h"
#include "ui_dialog.h"
#include "client.h"
#include "historique.h"
#include "histo.h"
#include "qrcode.h"
#include "smtp.h"
#include <QMessageBox>
#include <QDate>
#include <QIntValidator>
#include <QRegExpValidator>
#include <QDebug>
#include<QWidget>
#include <QTextDocument>
#include <QTextEdit>
#include <fstream>
#include <QTextStream>
#include <QRadioButton>
#include <QFileDialog>
#include <QPixmap>
#include <QPainter>
#include <QtSvg/QSvgRenderer>
#include <QtSvg/QSvgGenerator>
#include <QDirModel>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QAbstractPrintDialog>
#include <QDirModel>
#include <QtPrintSupport/QPrintDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QErrorMessage>
#include <QFile>
#include <QDataStream>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->affichageClient->setModel(Etmp.afficher());
    ui->affichageProduit_3->setModel(Etmp_h.afficherP());
    connect(ui->envoyer, SIGNAL(clicked()),this, SLOT(sendMail()));  // Ajouter Boutton
    connect(ui->browse, SIGNAL(clicked()), this, SLOT(browse())); // Ajouter Boutton

     combo();

     // Controle saisie
     ui->ajouter_cin_4->setValidator(new QIntValidator(0,999999,this));

     QRegExp rx("[A-Z][a-z]{0,30}");
     ui->ajouter_nom_4->setValidator(new QRegExpValidator(rx,this));
     ui->ajouter_prenom_4->setValidator(new QRegExpValidator(rx,this));
     ui->ajouter_adresse_4->setValidator(new QRegExpValidator(rx,this));

     QRegExp rxM("[a-z]{0,10}@[a-z]{0,10}.[a-z]{0,10}");
     ui->ajouter_adresseMail_4->setValidator(new QRegExpValidator(rxM,this));

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_ajouterClient_4_clicked()
{
        int res=ui->ajouter_cin_4->text().toInt();
        QString nom=ui->ajouter_nom_4->text();
        QString prenom=ui->ajouter_prenom_4->text();
        QString adresse=ui->ajouter_adresse_4->text();
        QString adresse_mail=ui->ajouter_adresseMail_4->text();
        QString date_naissance=ui->ajouter_dateN_4->text();


        Clients c(res,nom, prenom, adresse, adresse_mail, date_naissance);

        bool test=c.ajouter();

        if(test)
        {
            ui->affichageClient->setModel(Etmp.afficher());  //Actualiser

            QMessageBox::information(nullptr,QObject::tr("OK"),QObject::tr("Ajout effectué! \n" "Click cancel to exit"),QMessageBox::Cancel);
            histor.save("cin :"+ui->ajouter_cin_4->text(),"nom :"+ui->ajouter_nom_4->text(),"prenom :"+ui->ajouter_prenom_4->text(),"adresse :"+ui->ajouter_adresse_4->text(),"adresse mail :"+ui->ajouter_adresseMail_4->text());

        }
        else
        {
            QMessageBox::critical(nullptr,QObject::tr("Not OK"),QObject::tr("Ajout non effectué! \n" "Click cancel to exit"),QMessageBox::Cancel);
        }
}



void Dialog::on_supprimerClient_4_clicked()
{
    int cin =ui->cinClient_8->text().toInt();
    bool test=Etmp.supprimer(cin);

    if(test)
    {
        ui->affichageClient->setModel(Etmp.afficher());  //Actualiser

        QMessageBox::information(nullptr,QObject::tr("OK"), QObject::tr("Suppression effectuée \n" "Click cancel to exit"), QMessageBox::Cancel);
        histor1.save1("SUPPRIMER","cin :"+ui->cinClient_8->text());

    }
    else
    {
        QMessageBox::critical(nullptr,QObject::tr("Not OK"), QObject::tr("Suppression non effectuée \n" "Click cancel to exit"), QMessageBox::Cancel);
    }
}



void Dialog::on_modifierClient_4_clicked()
{
    int res=ui->cinClient_8->text().toInt();
    QString nom=ui->modifier_nom_4->text();
    QString prenom=ui->modifier_prenom_4->text();
    QString adresse=ui->modifier_adresse_4->text();
    QString adresse_mail=ui->modifier_adresseMail_4->text();
    QString date_naissance=ui->modifier_dateN_4->text();


    Clients c(res,nom, prenom, adresse, adresse_mail, date_naissance);

    bool test=c.modifier();

    if(test)
    {
        ui->affichageClient->setModel(Etmp.afficher());  //Actualiser

        QMessageBox::information(nullptr,QObject::tr("OK"),QObject::tr("Modification effectuée! \n" "Click cancel to exit"),QMessageBox::Cancel);
        histor.save2("SUPPRIMER","cin :"+ui->cinClient_8->text(),"nom :"+ui->modifier_nom_4->text(),"prenom :"+ui->modifier_prenom_4->text(),"adresse :"+ui->modifier_adresse_4->text(),"adresse mail :"+ui->modifier_adresseMail_4->text());

    }
    else
    {
        QMessageBox::critical(nullptr,QObject::tr("Not OK"),QObject::tr("Modification non effectuée! \n" "Click cancel to exit"),QMessageBox::Cancel);
    }

}

//Historique

void Dialog:: combo()
{
    QSqlQueryModel *m=new QSqlQueryModel();
              QSqlQuery *querry=new QSqlQuery();
              querry->prepare("SELECT cin FROM client");
              querry->exec();
              m->setQuery(*querry);
              ui->cin_client->setModel(m);
}

void Dialog::on_ajouterProduit_clicked()
{
    int cin= ui->cin_client->currentText().toInt();
    QString nom_produit= ui->nom_produit->text();
    QString date_achat= ui->date_achat->text();

    historique h(cin,nom_produit,date_achat);

    bool test=h.ajouterP();

    if (test)
    {
        ui->affichageProduit_3->setModel(Etmp_h.afficherP());

        QMessageBox::information(nullptr,QObject::tr("OK"),QObject::tr("Ajout effectué! \n" "Click cancel to exit"),QMessageBox::Cancel);
    }
    else
    {
        QMessageBox::critical(nullptr,QObject::tr("Not OK"),QObject::tr("Ajout non effectué! \n" "Click cancel to exit"),QMessageBox::Cancel);
    }
}

void Dialog::on_supprimerAchat_clicked()
{
    int id = ui->id_Achat->text().toInt();
    bool test=Etmp_h.supprimerP(id);

    if(test)
    {
        ui->affichageProduit_3->setModel(Etmp_h.afficherP());
        QMessageBox::information(nullptr,QObject::tr("OK"),QObject::tr("Suppression effectuée! \n" "Click cancel to exit"),QMessageBox::Cancel);
    }
    else
    {
        QMessageBox::critical(nullptr,QObject::tr("Not OK"), QObject::tr("Suppression non effectuée \n" "Click cancel to exit"), QMessageBox::Cancel);
    }
}


void Dialog::on_modifierProduit_3_clicked()
{
    int res= ui->id_Achat->text().toInt();
    QString nom_produit=ui->modifier_nomProduit_3->text();
    QString date_achat=ui->modifier_dateAchat_3->text();

    historique h(res,nom_produit,date_achat);

    bool test=h.modifierP();

    if (test)
    {
        ui->affichageProduit_3->setModel(Etmp_h.afficherP());

        QMessageBox::information(nullptr,QObject::tr("OK"),QObject::tr("Modification effectuée! \n" "Click cancel to exit"),QMessageBox::Cancel);
    }
    else
    {
        QMessageBox::critical(nullptr,QObject::tr("Not OK"),QObject::tr("Modification non effectuée! \n" "Click cancel to exit"),QMessageBox::Cancel);
    }
}

//Metiers

void Dialog::on_rechercherClient_4_clicked()
{
    QString nom=ui->recherche_4->text();
    if(nom=="")
    {
        QMessageBox::information(nullptr,QObject::tr("Champ vide"),QObject::tr("Veuiller insérer un NOM!"),QMessageBox::Cancel);
        ui->affichageClient->setModel(Etmp.afficher());
    }
    else
    {
        ui->affichageClient->setModel(Etmp.rechercherNom(nom));
    }
}



void Dialog::on_tri_nom_clicked()
{
    ui->affichageClient->setModel(Etmp.triNom());
}


void Dialog::on_QRCode_clicked()
{
    if(ui->affichageClient->currentIndex().row()==-1)
        QMessageBox::information(nullptr, QObject::tr("Suppression"),
                                 QObject::tr("Veuillez Choisir un employé du Tableau.\n"
                                             "Click Ok to exit."), QMessageBox::Ok);
    else
    {
         int  Code=ui->affichageClient->model()->data(ui->affichageClient->model()->index(ui->affichageClient->currentIndex().row(),0)).toInt();
        //const QrCode qr = QrCode::encodeText(std::to_string(Code).c_str(), QrCode::Ecc::LOW);
         const qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(std::to_string(Code).c_str(),qrcodegen::QrCode::Ecc::LOW);
         std::ofstream myfile;
         myfile.open ("qrcode.svg") ;
         myfile << qr.toSvgString(1);
         myfile.close();
         QSvgRenderer svgRenderer(QString("qrcode.svg"));
         QPixmap pix( QSize(90, 90) );
         QPainter pixPainter( &pix );
         svgRenderer.render(&pixPainter);
         ui->label_6->setPixmap(pix);
    }
}


void  Dialog::browse()
{
    files.clear();

    QFileDialog dialog(this);
    dialog.setDirectory(QDir::homePath());
    dialog.setFileMode(QFileDialog::ExistingFiles);

    if (dialog.exec())
        files = dialog.selectedFiles();

    QString fileListString;
    foreach(QString file, files)
        fileListString.append( "\"" + QFileInfo(file).fileName() + "\" " );

    ui->file->setText( fileListString );

}

void   Dialog::sendMail()
{
    Smtp* smtp = new Smtp("moeness.gannouni@esprit.tn",ui->pass->text(), "smtp.gmail.com");
    connect(smtp, SIGNAL(status(QString)), this, SLOT(mailSent(QString)));

    if( !files.isEmpty() )
        smtp->sendMail("moeness.gannouni@esprit.tn", ui->adresseMail->text() , ui->objet->text(),ui->message->toPlainText(), files );
    else
        smtp->sendMail("moeness.gannouni@esprit.tn", ui->adresseMail->text() , ui->objet->text(),ui->message->toPlainText());
}

void   Dialog::mailSent(QString status)
{

    if(status == "Message sent")
        QMessageBox::warning( nullptr, tr( "Qt Simple SMTP client" ), tr( "Message sent!\n\n" ) );
    ui->adresseMail->clear();
    ui->objet->clear();
    ui->file->clear();
    ui->message->clear();
    ui->pass->clear();
}



void Dialog::on_browse_clicked()
{
  // browse();

}

void Dialog::on_envoyer_clicked()
{
   /* QString status;
    sendMail();
    mailSent(status);
    */
}


void Dialog::on_pdf_6_clicked()
{
            QPrinter printer;
            printer.setPrinterName("Printer Name");

            QPrintDialog pDialog(&printer, this);

            if(pDialog.exec() == QDialog::Rejected){
                QMessageBox::warning(this, "Warning", "Cannot Access Printer");
                return;
            }
            ui->affichageClient->render(&printer);

}

void Dialog::on_afficherHisto_clicked()
{
    QFile file("C:/Users/MOEµNESS/Desktop/QT/Tache clients/tache_clients/historique.txt");
        if (!file.open(QIODevice::ReadOnly))
            QMessageBox::information(0,"info",file.errorString());
        QTextStream in(&file);
        ui->textBrowser->setText(in.readAll());
}