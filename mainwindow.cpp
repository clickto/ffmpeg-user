#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QThread>
QString name, w, h, maxsize;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_load_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "",
                                                    tr("Data (*.*)"));
    ui->lineEdit_src->setText(fileName);

}

void MainWindow::on_pushButton_encode_clicked()
{
    //read data file
    QString src = ui->lineEdit_src->text();
    int  t_srcFmt = ui->comboBox_srcFmt->currentIndex();
    QFile file(src);
    if (!file.open(QFile::ReadOnly)) {
        qDebug() << "open src file failed" << file.errorString();
        return ;
    }
if(!t_srcFmt){

    name    = QString::fromLocal8Bit(file.readLine());
    w       = QString::fromLocal8Bit(file.readLine());
    maxsize = QString::fromLocal8Bit(file.readLine());
}
    QByteArray data = file.readAll();
    qDebug()<<"read pgm"  <<name << w << h << maxsize << data.size();
//    qDebug()<<"data size:"<<data.size()<<endl;
    file.close();
    //open out file.
    QString encodeFileName = ui->lineEdit_encodeFileName->text();
    QFile out(encodeFileName);
    if (!out.open(QFile::WriteOnly)) {
        qDebug() << "open encode file failed" << out.errorString();
        return ;
    }
    //init encode params
    Params params;
    params.width    = ui->lineEdit_width->text().toInt();
    params.height   = ui->lineEdit_height->text().toInt();
    params.bit_rate = ui->lineEdit_bitrate->text().toInt();
    params.fps      = ui->lineEdit_fps->text().toInt();
    params.gop_size = ui->lineEdit_gopsize->text().toInt();
    params.max_b_frames = ui->lineEdit_maxbframe->text().toInt();
    params.srcFmt   = ui->comboBox_srcFmt->currentIndex();
    params.destFmt  = ui->comboBox_destFmt->currentIndex();
    params.codecID  = AV_CODEC_ID_H264;

    encode = new FFUser;
    encode->initEncode(params);
    //prepare for write encode result
    static int num = 0;
    connect(encode, &FFUser::encodeOneFrame, [&](QByteArray frame){
        out.write(frame);
        qDebug() << "get encode frame" << num;
        num++;
    });

    int offset = params.width * params.height;
    char *str = new char[offset];
    char *t_p = data.data();
    memset(str,0x00,offset);
    memcpy(str,t_p + data.size()-offset,offset);
    data.append((const char*)str,offset);

    unsigned char *p = (unsigned char *)data.data();

    int t_tmp = encode->encode(p,data.size());
    qDebug()<<"cout:"<<t_tmp<<endl;
    out.close();
    delete encode;
    delete []str;
}

void MainWindow::on_pushButton_decode_clicked()
{
    //read data file
    QString src = ui->lineEdit_encodeFileName->text();
    QFile file(src);
    if (!file.open(QFile::ReadOnly)) {
        qDebug() << "open encode file failed" << file.errorString();
        return ;
    }
    QByteArray data = file.readAll();
    file.close();

    //open out file.
    QString decodeFileName = ui->lineEdit_decodeFileName->text();
    QFile out(decodeFileName);
    if (!out.open(QFile::WriteOnly)) {
        qDebug() << "open decode file failed" << out.errorString();
        return ;
    }
    //init decode params
    Params params;
    params.width    = ui->lineEdit_width->text().toInt();
    params.height   = ui->lineEdit_height->text().toInt();
    params.bit_rate = ui->lineEdit_bitrate->text().toInt();
    params.fps      = ui->lineEdit_fps->text().toInt();
    params.gop_size = ui->lineEdit_gopsize->text().toInt();
    params.max_b_frames = ui->lineEdit_maxbframe->text().toInt();
    params.srcFmt   = ui->comboBox_srcFmt->currentIndex();
    params.destFmt  = ui->comboBox_destFmt->currentIndex();
    params.codecID  = AV_CODEC_ID_H264;

    decode = new FFUser;
    decode->initDecode(params);

    QByteArray head;
    head.append(name);
    head.append(w);
    head.append(maxsize);
    out.write(head);

    //prepare for write decode result
    connect(decode, &FFUser::decodeOneFrame, [&](QByteArray frame, int width, int height){
        Q_UNUSED(width);
        Q_UNUSED(height);
        out.write(frame);
    });

    decode->decode((const unsigned char *)data.data(), data.size());

    out.close();
    delete decode;
}
