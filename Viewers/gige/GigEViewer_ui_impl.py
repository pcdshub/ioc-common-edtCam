#! /bin/env python

import sys
import time
import logging
from PycaImage import PycaImage
from PyQt4.QtGui import QApplication
from PyQt4.QtGui import QPainter
from PyQt4.QtGui import QMainWindow
from PyQt4.QtGui import QWidget
from PyQt4.QtCore import QRect
from PyQt4.QtGui import QPen, QBrush
from PyQt4.QtCore import Qt
from PyQt4.QtCore import SIGNAL
from GigEViewer_ui import Ui_MainWindow
from pyca_widgets import *


class DisplayImage(QWidget):
    def __init__(self, parent, img, gui):
        QWidget.__init__(self, parent)
        self.parent = parent
        self.image = img.img
        self._gui = gui
        self.scaled_image = img.img
        self.xoff = 0
        self.yoff = 0
        self.scale = 1.0
        self.old_width = 0
        self.old_height = 0
        self.roiXoff = 0
        self.roiYoff = 0
        self.painter = QPainter()
        img.set_new_image_callback(self.set_image)
        self.updates = 0
        self.last_updates = 0
        self.last_time = time.time()
        self.rateTimer = QTimer()
        self.connect(self.rateTimer, SIGNAL("timeout()"), self.calcDisplayRate)
        self.rateTimer.start(1000)

    def paintEvent(self, event):
        self.painter.begin(self)
        if self.scaled_image:
            self.painter.drawImage(self.xoff, self.yoff, self.scaled_image)
            try:
                x = int(self._gui.leCross1X.text())
                y = int(self._gui.leCross1Y.text())
                color = self._gui.cbCross1Color.currentIndex()
                # logging.debug("cross1: x=%d y=%d color=%d", x, y, color)
                if (color != 0) and self.isCrossOnROIImage(x, y):
                    self.drawCross(self.painter, x, y, color)
            except Exception, e:
                logging.debug("%s", e)
            try:
                x = int(self._gui.leCross2X.text())
                y = int(self._gui.leCross2Y.text())
                color = self._gui.cbCross2Color.currentIndex()
                # logging.debug("cross2: x=%d y=%d color=%d", x, y, color)
                if (color != 0) and self.isCrossOnROIImage(x, y):
                    self.drawCross(self.painter, x, y, color)
            except Exception, e:
                logging.debug("%s", e)
        self.painter.end()
        self.updates += 1

    def isCrossOnROIImage(self, x, y):
        # logging.debug("ROI: x=%d y=%d w=%d h=%d", self.roiXoff, self.roiYoff, self.image.width(), self.image.height())
        # logging.debug("%s", self.roiXoff <= x and x <= self.roiXoff + self.image.width() and self.roiYoff <= y and y <= self.roiYoff + self.image.height())
        return self.roiXoff <= x and x <= self.roiXoff + self.image.width() and \
               self.roiYoff <= y and y <= self.roiYoff + self.image.height()

    def drawCross(self, qp, x, y, color):
        # logging.debug("color = %d", color)
        if color == 0:
            color = None
        elif color == 1:
            color = Qt.black
        elif color == 2:
            color = Qt.red
        elif color == 3:
            color = Qt.green
        elif color == 4:
            color = Qt.blue
        elif color == 5:
            color = Qt.white
        width = 1
        if color != None:
            style = Qt.SolidLine
            pen = QPen(QBrush(color), width, style)
            qp.setPen(pen)
            x -= self.roiXoff
            y -= self.roiYoff
            x *= self.scale
            y *= self.scale
            x += self.xoff
            y += self.yoff
            # logging.debug("x=%d  y=%d", x, y)
            qp.drawLine( x-5, y, x+5, y)
            qp.drawLine( x, y-5, x, y+5)

    def resizeEvent(self, event):
        if self.image:
            # logging.debug("frame size: %d x %d", self.width(), self.height())
            self.scaled_image = self.image.scaled(self.width(), self.height(), aspectRatioMode = Qt.KeepAspectRatio)
            self.xoff = ( self.width() - self.scaled_image.width() ) / 2
            self.yoff = ( self.height() - self.scaled_image.height() ) / 2
            self.scale = float(self.scaled_image.width()) / self.image.width()
            # logging.debug("img xoff = %f", self.xoff)
            # logging.debug("img yoff = %f", self.yoff)
            # logging.debug("img scale = %f", self.scale)

    def set_image(self, img):
        self.setGeometry(QRect(0, 0, self.parent.width(), self.parent.height()))
        self.image = img
        if self.image and not self.image.isNull():
            self.scaled_image = self.image.scaled(self.width(), self.height(), aspectRatioMode = Qt.KeepAspectRatio)
            if self.image.width() != self.old_width or self.image.height() != self.old_height:
                # logging.debug("img:  w=%d  h=%d", self.image.width(), self.image.height())
                self.old_width = self.image.width()
                self.old_height = self.image.height()
                # logging.debug("scaled img:  w=%d  h=%d", self.scaled_image.width(), self.scaled_image.height())
                self.xoff = ( self.width() - self.scaled_image.width() ) / 2
                self.yoff = ( self.height() - self.scaled_image.height() ) / 2
                self.scale = float(self.scaled_image.width()) / self.image.width()
                # logging.debug("img xoff = %f", self.xoff)
                # logging.debug("img yoff = %f", self.yoff)
                # logging.debug("img scale = %f", self.scale)
        self.update()

    def calcDisplayRate(self):
        now = time.time()
        updates = self.updates - self.last_updates
        delta = now - self.last_time
        rate = updates/delta
        self._gui.label_rate.setText('%.1f' % rate)
        self._gui.label_rate.repaint()
        self.last_time = now
        self.last_updates = self.updates


class GigEImageViewer(QMainWindow, Ui_MainWindow):
    myImgCounterLab = None
    myImgRateLab    = None
    myExpTimeLab    = None
    myExpPeriodLab  = None
    myGainLab       = None
    myExpTimeLE     = None
    myExpPeriodLE   = None
    myGainLE        = None
    myROIMinXLab    = None
    myROISizeXLab   = None
    myROIMinYLab    = None
    myROISizeYLab   = None
    myROIMinXLE     = None
    myROISizeXLE    = None
    myROIMinYLE     = None
    myROISizeYLE    = None
    myBinXLab       = None
    myBinXLE        = None
    myImgModeCB     = None
    myTriggerModeCB = None
    myCross1XLE     = None
    myCross1YLE     = None
    myCross2XLE     = None
    myCross2YLE     = None
    myCross1CB      = None
    myCross2CB      = None
    myStartB        = None
    myStopB         = None

    def __init__(self, pv_name):
        QMainWindow.__init__(self)
        self.setupUi(self)

        self.cam_pv = pv_name
        image_pv = pv_name.replace( 'cam', 'image' )
        image_pv = image_pv.replace( 'CAM', 'IMAGE' )

        self.img = PycaImage(image_pv)

        self.display_image = DisplayImage(self.wImage, self.img, self)

        # FIXME - shouldn't these all be MinX_RBV
        # NOTE:  there is also a bug in the ROI plugin--the _RBV values could be wrong if out of range
        self.display_image.roiXoff = self.pv_get(self.cam_pv+':MinX')
        self.display_image.roiYoff = self.pv_get(self.cam_pv+':MinY')
        # logging.debug("roiXoff = %d", self.display_image.roiXoff)
        # logging.debug("roiYoff = %d", self.display_image.roiYoff)

        self.lPV.setText(self.cam_pv)
        GigEImageViewer.myImgCounterLab = PycaLabel      (image_pv+':ArrayCounter_RBV', self.lImgCounter)
        GigEImageViewer.myImgRateLab    = PycaLabel      (image_pv+':ArrayRate_RBV',    self.lImgRate)
        GigEImageViewer.myExpTimeLab    = PycaLabel      (self.cam_pv+':AcquireTime_RBV',    self.lExpTime)
        GigEImageViewer.myExpPeriodLab  = PycaLabel      (self.cam_pv+':AcquirePeriod_RBV',  self.lExpPeriod)
        GigEImageViewer.myGainLab       = PycaLabel      (self.cam_pv+':Gain_RBV',           self.lGain)
        GigEImageViewer.myExpTimeLE     = PycaLineEdit   (self.cam_pv+':AcquireTime',        self.leExpTime)
        GigEImageViewer.myExpPeriodLE   = PycaLineEdit   (self.cam_pv+':AcquirePeriod',      self.leExpPeriod)
        GigEImageViewer.myGainLE        = PycaLineEdit   (self.cam_pv+':Gain',               self.leGain)
        GigEImageViewer.myROIMinXLab    = PycaLabel      (self.cam_pv+':MinX_RBV',           self.lRoiXStart)
        GigEImageViewer.myROISizeXLab   = PycaLabel      (self.cam_pv+':SizeX_RBV',          self.lRoiXSize)
        GigEImageViewer.myROIMinYLab    = PycaLabel      (self.cam_pv+':MinY_RBV',           self.lRoiYStart)
        GigEImageViewer.myROISizeYLab   = PycaLabel      (self.cam_pv+':SizeY_RBV',          self.lRoiYSize)
        GigEImageViewer.myROIMinXLE     = PycaLineEdit   (self.cam_pv+':MinX',               self.leRoiXStart)
        self.leRoiXStart.editingFinished.connect(self.roiXStartChanged)
        GigEImageViewer.myROISizeXLE    = PycaLineEdit   (self.cam_pv+':SizeX',              self.leRoiXSize)
        GigEImageViewer.myROIMinYLE     = PycaLineEdit   (self.cam_pv+':MinY',               self.leRoiYStart)
        self.leRoiYStart.editingFinished.connect(self.roiYStartChanged)
        GigEImageViewer.myROISizeYLE    = PycaLineEdit   (self.cam_pv+':SizeY',              self.leRoiYSize)
        GigEImageViewer.myBinXLE        = PycaLineEdit   (self.cam_pv+':BinX',               self.leBinX)
        GigEImageViewer.myBinXLab       = PycaLabel      (self.cam_pv+':BinX_RBV',           self.lBinX)
        self.leBinX.editingFinished.connect(self.setBinning)
        imageModes = ('Single', 'Multiple', 'Continuous')
        GigEImageViewer.myImgModeCB     = PycaComboBox   (self.cam_pv+':ImageMode',          self.cbImageMode, items = imageModes)
        triggerModes = ('Free Run', 'Sync In 1', 'Sync In 2', 'Sync In 3',
                        'Sync In 4', 'Fixed Rate', 'Software')
        GigEImageViewer.myTriggerModeCB = PycaComboBox   (self.cam_pv+':TriggerMode',        self.cbTriggerMode, items = triggerModes)
        GigEImageViewer.myCross1XLE     = PycaLineEdit   (self.cam_pv+':Cross1X',            self.leCross1X)
        GigEImageViewer.myCross1YLE     = PycaLineEdit   (self.cam_pv+':Cross1Y',            self.leCross1Y)
        GigEImageViewer.myCross2XLE     = PycaLineEdit   (self.cam_pv+':Cross2X',            self.leCross2X)
        GigEImageViewer.myCross2YLE     = PycaLineEdit   (self.cam_pv+':Cross2Y',            self.leCross2Y)
        colors          = ('None', 'Black', 'Red', 'Green', 'Blue', 'White')
        GigEImageViewer.myCross1CB      = PycaComboBox   (self.cam_pv+':Cross1Color',        self.cbCross1Color, items = colors)
        GigEImageViewer.myCross2CB      = PycaComboBox   (self.cam_pv+':Cross2Color',        self.cbCross2Color, items = colors)
        GigEImageViewer.myStartB        = PycaPushButton (self.cam_pv+':Acquire',            self.bStart,        value = 1)
        GigEImageViewer.myStopB         = PycaPushButton (self.cam_pv+':Acquire',            self.bStop,         value = 0)

    def pv_get(self, pv_name):
        # logging.debug(pv_name)
        pv = Pv(pv_name)
        try:
            pv.connect(timeout=1.0)
            # logging.debug("connected")
            pv.get(False, 0.1)
            # logging.debug("value = %d", pv.value)
            return pv.value
        except Exception, e:
            # logging.debug("exception %s", e)
            return 0

    def pv_put(self, pv_name, val):
        # logging.debug("%s, %d", pv_name, val)
        pv = Pv(pv_name)
        try:
            pv.connect(timeout=1.0)
            # logging.debug("connected")
            pv.put(val)
        except Exception, e:
            # logging.debug("exception %s", e)
            pass

    def setBinning(self):
        # Called when the user enters a number for binning
        try:
            val = int(self.leBinX.text())
            self.pv_put(self.cam_pv+':BinY', val)
        except Exception, e:
            # logging.debug("e = %s", e)
            pass

    def roiXStartChanged(self):
        # Called when the user enters a number for X start of ROI
        try:
            self.display_image.roiXoff = int(self.leRoiXStart.text())
            # logging.debug("roiXoff = %d", self.display_image.roiXoff)
        except Exception, e:
            # logging.debug("e = %s", e)
            pass

    def roiYStartChanged(self):
        # Called when the user enters a number for Y start of ROI
        try:
            self.display_image.roiYoff = int(self.leRoiYStart.text())
            # logging.debug("roiYoff = %d", self.display_image.roiYoff)
        except Exception, e:
            # logging.debug("e = %s", e)
            pass

    def __del__(self):
        self.img.disconnect()
        GigEImageViewer.myImgCounterLab = None
        GigEImageViewer.myImgRateLab    = None
        GigEImageViewer.myExpTimeLab    = None
        GigEImageViewer.myExpPeriodLab  = None
        GigEImageViewer.myGainLab       = None
        GigEImageViewer.myExpTimeLE     = None
        GigEImageViewer.myExpPeriodLE   = None
        GigEImageViewer.myGainLE        = None
        GigEImageViewer.myROIMinXLab    = None
        GigEImageViewer.myROISizeXLab   = None
        GigEImageViewer.myROIMinYLab    = None
        GigEImageViewer.myROISizeYLab   = None
        GigEImageViewer.myROIMinXLE     = None
        GigEImageViewer.myROISizeXLE    = None
        GigEImageViewer.myROIMinYLE     = None
        GigEImageViewer.myROISizeYLE    = None
        GigEImageViewer.myBinXLab       = None
        GigEImageViewer.myBinXLE        = None
        GIGEImageViewer.myImgModeCB     = None
        GIGEImageViewer.myTriggerModeCB = None
        GigEImageViewer.myCross1XLE     = None
        GigEImageViewer.myCross1YLE     = None
        GigEImageViewer.myCross2XLE     = None
        GigEImageViewer.myCross2YLE     = None
        GigEImageViewer.myCross1CB      = None
        GigEImageViewer.myCross2CB      = None
        GigEImageViewer.myStartB        = None
        GigEImageViewer.myStopB         = None


def main():
    from options import Options
    options = Options(['camerapv'], [], [])
    try:
        options.parse()
    except Exception, msg:
        options.usage(str(msg))
        sys.exit()

    app = QApplication(sys.argv)

    win = GigEImageViewer(options.camerapv)
    win.resize(1080, 687)

    sys.setcheckinterval(1000) # default is 100
    win.show()

    sys.exit(app.exec_())

if __name__ == '__main__':
    logging.basicConfig(format='%(filename)s:%(lineno)d:%(levelname)-8s %(funcName)s: %(message)s',
                        level=logging.DEBUG)
    main()
