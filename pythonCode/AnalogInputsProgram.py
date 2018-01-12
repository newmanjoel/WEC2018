# -*- coding: utf-8 -*-
"""
Created on Thu Jan 11 15:04:09 2018

@author: joell
"""

from PyQt4 import QtCore, QtGui, uic  # Import the PyQt4 module we'll need
import sys  # We need sys so that we can pass argv to QApplication

class MainScreen(QtGui.QMainWindow):
    def __init__(self):
        '''
        This is the main screen that the user interacts with.

        '''
        super(self.__class__, self).__init__()
        uic.loadUi('AnalogInputsScreen.ui', self)

    def keyPressEvent(self, event):

        if type(event) == QtGui.QKeyEvent:
            #print"KeyText:{} KeyNumber:{}".format(event.text(),event.key())
            #here accept the event and do something
            #print event.key()
            if(event.key()==16777235):
                print "Up Arrow"
            elif(event.key()==16777237):
                print "Down Arrow"
            elif(event.key()==16777234):
                print "Left Arrow"
            elif(event.key()==16777236):
                print "Right Arrow"
            event.accept()
        else:
            event.ignore()

if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)  # A new instance of QApplication
    form = MainScreen()  # We set the form to be our ExampleApp (design)
    form.show()  # Show the form
    app.exec_()  # and execute the app