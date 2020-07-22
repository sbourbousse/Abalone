#include "boutondirection.h"
#include "abalonegrid.h"
#include "mainwindow.h"
int BoutonDirection::getDx() const
{
    return dx;
}

int BoutonDirection::getDy() const
{
    return dy;
}

void BoutonDirection::setDx(int value)
{
    dx = value;
}

void BoutonDirection::setDy(int value)
{
    dy = value;
}

void BoutonDirection::envoyerDirection()
{
    QString laDirection;
    if(dx == 2 && dy == 0)
        laDirection = "E";
    else if (dx == -2 && dy == 0)
        laDirection = "O";
    else if (dx == 1 && dy == -1)
        laDirection = "NE";
    else if (dx == -1 && dy == -1)
        laDirection = "NO";
    else if (dx == 1 && dy == 1)
        laDirection = "SE" ;
    else if (dx == -1  && dy == 1)
        laDirection = "SO";

    emit(direction(laDirection));
}

BoutonDirection::BoutonDirection(QWidget* parent):QPushButton(parent)
{
    connect(this,SIGNAL(clicked()),this,SLOT(envoyerDirection()));
}
