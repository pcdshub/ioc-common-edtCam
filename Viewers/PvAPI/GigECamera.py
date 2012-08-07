#! /usr/bin/env python

import time
from ctypes import *
import struct
import logging
import numpy as np
from scipy import weave

def toColorArray(arr, img):
    code = """ 
    #line 13
    unsigned char *sp = (unsigned char *)arr;
    unsigned char *dp = (unsigned char *)img;
    unsigned char *endp = sp + size;
    int r, g, b;

    while (sp < endp) {
        r = *sp++;
        g = *sp++;
        b = *sp++;
        *dp++ = b;
        *dp++ = g;
        *dp++ = r;
        *dp++ = '0xff';
    }
    """

    logging.debug("l=%d %s", len(arr), repr(arr))
    size = len(arr)
    weave.inline(code, ["arr", "img", "size"])
    logging.debug("l=%d %s", len(img), repr(img))
    return img


class AttributeInfo(Structure):
    _fields_ = [
        ('dataType', c_ulong),
        ('flags', c_ulong),
        ('category', c_char_p),
        ('impact', c_char_p),
        ('reserved', c_ulong * 4)
        ]


class Frame(Structure):
    _fields_ = [
        # --- in ---
        ('imageBuffer', POINTER(c_char)),
        ('imageBufferSize', c_ulong),
        ('ancillaryBuffer', c_void_p),
        ('ancillaryBufferSize', c_ulong),
        ('context', c_void_p * 4),
        ('reserved1', c_ulong * 8),
        # --- out ---
        ('status', c_ulong),
        ('imageSize', c_ulong),
        ('ancillarySize', c_ulong),
        ('width', c_ulong),
        ('height', c_ulong),
        ('regionX', c_ulong),
        ('regionY', c_ulong),
        ('format', c_ulong),
        ('bitDepth', c_ulong),
        ('bayerPattern', c_ulong),
        ('frameCount', c_ulong),
        ('timestampLo', c_ulong),
        ('timestampHi', c_ulong),
        ('reserved2', c_ulong * 32)
        ]

    def __str__(self):
        return \
        "status: %d " %  self.status +\
        "imageSize: %d " %  self.imageSize +\
        "width: %d " %  self.width +\
        "height: %d " %  self.height +\
        "frameCount: %d" % self.frameCount


class GigECamera:
    """ Python thin interface to the AVT GigE camera API """

    api = cdll.LoadLibrary('./libPvAPI.so')

    @classmethod
    def errstr(cls, err):
        errors = [
            "ePvErrSuccess",
            "ePvErrCameraFault",
            "ePvErrInternalFault",
            "ePvErrBadHandle",
            "ePvErrBadParameter",
            "ePvErrBadSequence",
            "ePvErrNotFound",
            "ePvErrAccessDenied",
            "ePvErrUnplugged",
            "ePvErrInvalidSetup",
            "ePvErrResources",
            "ePvErrBandwidth",
            "ePvErrQueueFull",
            "ePvErrBufferTooSmall",
            "ePvErrCancelled",
            "ePvErrDataLost",
            "ePvErrDataMissing",
            "ePvErrTimeout",
            "ePvErrOutOfRange",
            "ePvErrWrongType",
            "ePvErrForbidden",
            "ePvErrUnavailable",
            "ePvErrFirewall"
        ]
        if err > len(errors):
            return "UNKNOWN"
        else:
            return errors[err]

    def __init__(self, callback):
        self.initialized = False
        self.opened = False
        self.capturing = False
        self.framesQueued = False
        self.running = False
        self.camHandle = c_void_p(0)
        self.width = 0
        self.height = 0
        self.bytes_per_frame = 0
        self.frames = []
        self.CBFUNC = CFUNCTYPE(None, POINTER(Frame))
        self.cb = self.CBFUNC(self.captureCB)
        self.frames_count = 0
        self.img = None
        self.callback = callback

    def __del__(self):
        logging.info("")
        self.stop()
        self.disconnect()

    def captureCB(self, frame):
        logging.info(frame[0].__str__())
        GigECamera.api.PvCaptureQueueFrame.argtypes = [ c_void_p, POINTER(Frame), self.CBFUNC ]
        if frame[0].status != 14:
            ret = GigECamera.api.PvCaptureQueueFrame(self.camHandle, frame, self.cb)
            if ret and ret != 5:
                logging.error("error code = %s" % GigECamera.errstr(ret))
        if frame[0].status == 0:
            size = frame[0].imageSize
            count = frame[0].frameCount
            width = frame[0].width
            height = frame[0].height
            # address of frame data
            p = np.core.multiarray.int_asbuffer(addressof(frame[0].imageBuffer.contents), size)
            # into numpy array
            arr = np.frombuffer(p, dtype=np.uint8, count=size)
            # logging.info("        l=%d %s", len(arr), repr(arr))
            img = toColorArray(arr, self.img)
            logging.info("        l=%d %s", len(img), repr(img))

            self.frames_count += 1

            if self.callback:
                if self.running:
                    self.callback(img, count, width, height)

    def PvVersion(self):
        """ Return the major and minor version number of the PvAPI module """
        # logging.debug("")
        major = c_int(0)
        minor = c_int(0)
        GigECamera.api.PvVersion.argtypes = [ POINTER(c_int), POINTER(c_int) ]
        GigECamera.api.PvVersion(byref(major), byref(minor))
        logging.info("%d.%d" % (major.value, minor.value))
        return major.value, minor.value

    def PvInitialize(self):
        """
        Initialize the PvAPI module. You can't call any PvAPI functions,
        other than PvVersion, until the module is initialized.
        """
        logging.info("")
        ret = GigECamera.api.PvInitialize()
        if ret:
            logging.error("error code = %s" % GigECamera.errstr(ret))
            raise Exception("PVInitialize failed. error = %s" % GigECamera.errstr(ret))

    def PvInitializeNoDiscovery(self):
        """
        Initialize the PvAPI module. You can't call any PvAPI functions,
        other than PvVersion, until the module is initialized.
        """
        logging.info("")
        ret = GigECamera.api.PvInitializeNoDiscovery()
        if ret:
            logging.error("error code = %s" % GigECamera.errstr(ret))
            raise Exception("PVInitializeNoDiscovery failed. error = %s" % GigECamera.errstr(ret))

    def PvUnInitialize(self):
        """
        Un-initialize the PvAPI module.
        This frees system resources used by PvAPI
        """
        logging.info("")
        GigECamera.api.PvUnInitialize()

    def PvCameraOpenByAddr(self, ipaddr):
        """
        Open a camera using its IP address.
        This function can be used to open a GigE Vision camera located on a
        different IP subnet.
        """
        import socket
        logging.info("ip = %s" % ipaddr)
        ip = socket.inet_aton(ipaddr)
        ip = struct.unpack("I", ip)
        cipaddr = c_int(ip[0])
        accessflags = c_int(4)
        GigECamera.api.PvCameraOpenByAddr.argtypes = [ c_int, c_int, POINTER(c_void_p) ]
        ret = GigECamera.api.PvCameraOpenByAddr(cipaddr, accessflags, byref(self.camHandle))
        if ret:
            logging.error("error code = %s" % GigECamera.errstr(ret))
            raise Exception("PVCameraOpenByAddr(%s) failed. error = %s" % (ipaddr, GigECamera.errstr(ret)))

    def PvCameraClose(self):
        """ Close a camera. """
        logging.info("")
        GigECamera.api.PvCameraClose.argtypes = [ c_void_p ]
        ret = GigECamera.api.PvCameraClose(self.camHandle)
        if ret:
            logging.error("error code = %s" % GigECamera.errstr(ret))
            raise Exception("PVCameraClose failed. error = %s" % GigECamera.errstr(ret))

    def PvCaptureStart(self):
        """
        Start the image capture stream. This initializes both the camera and
        the host in preparation to capture acquired images.
        """
        logging.info("")
        ret = GigECamera.api.PvCaptureStart(self.camHandle)
        if ret:
            logging.error("error code = %s" % GigECamera.errstr(ret))
            raise Exception("PVCapturStart failed. error = %s" % GigECamera.errstr(ret))

    def PvCaptureEnd(self):
        """
        Shut down the image capture stream.
        This resets the FrameCount parameter.
        """
        logging.info("")
        ret = GigECamera.api.PvCaptureEnd(self.camHandle)
        if ret:
            logging.error("error code = %s" % GigECamera.errstr(ret))
            raise Exception("PVCaptureEnd failed. error = %s" % GigECamera.errstr(ret))

    def PvCaptureQueueFrames(self):
        """
        Place an image buffer onto the frame queue. This function returns
        immediately; it does not wait until the frame has been captured.
        """
        logging.info("")
        for i in range(4):
            f = Frame()
            buf = create_string_buffer( self.bytes_per_frame )
            f.imageBuffer = buf
            f.imageBufferSize = self.bytes_per_frame
            # logging.info("addr of buf: %s\n", addressof(f.imageBuffer.contents))
            f.ancillaryBufferSize = 0
            self.frames.append(f)
            # GigECamera.api.PvCaptureQueueFrame.argtypes = [ c_void_p, POINTER(Frame), c_void_p ]
            # ret = GigECamera.api.PvCaptureQueueFrame(self.camHandle, f, None)
            GigECamera.api.PvCaptureQueueFrame.argtypes = [ c_void_p, POINTER(Frame), self.CBFUNC ]
            ret = GigECamera.api.PvCaptureQueueFrame(self.camHandle, f, self.cb)
            if ret:
                break
        if ret:
            logging.error("error code = %s" % GigECamera.errstr(ret))
            raise Exception("PVCaptureQueueFrames failed. error = %s" % GigECamera.errstr(ret))

    def PvCaptureWaitForFrameDone(self, f):
        """ Block the calling thread until a frame is complete """
        GigECamera.api.PvCaptureWaitForFrameDone.argtypes = [ c_void_p, POINTER(Frame), c_int ]
        ret = GigECamera.api.PvCaptureWaitForFrameDone(self.camHandle, f, 2000)
        if ret:
            logging.error("error code = %s" % GigECamera.errstr(ret))
            raise Exception("PVCaptureWaitForFrameDone failed. error = %s" % GigECamera.errstr(ret))
        logging.info(f.__str__())
        # logging.info("addr of buf: %s\n", addressof(f.imageBuffer.contents))
        p = np.core.multiarray.int_asbuffer(addressof(f.imageBuffer.contents), f.imageSize)
        a = np.frombuffer(p, dtype=np.uint8, count=f.imageSize)
        # logging.info("%d\n", len(a))
        logging.info("        %s", repr(a))
        self.frames_count += 1

        GigECamera.api.PvCaptureQueueFrame.argtypes = [ c_void_p, POINTER(Frame), c_void_p ]
        # GigECamera.api.PvCaptureQueueFrame.argtypes = [ c_void_p, POINTER(Frame), self.CBFUNC ]
        ret = GigECamera.api.PvCaptureQueueFrame(self.camHandle, f, None)
        if ret:
            logging.error("error code = %s" % GigECamera.errstr(ret))
            raise Exception("PVCaptureQueueFrame failed. error = %s" % GigECamera.errstr(ret))

    def PvCaptureQueueClear(self):
        """
        Clear queued frames. Aborts actively written frame with
        pFrame->Status = ePvErrDataMissing.
        Further queued frames returned with pFrame->Status = ePvErrCancelled.
        """
        logging.info("")
        GigECamera.api.PvCaptureQueueClear.argtypes = [ c_void_p ]
        ret = GigECamera.api.PvCaptureQueueClear(self.camHandle)
        if ret:
            logging.error("error code = %s" % GigECamera.errstr(ret))
            # raise Exception("PVCaptureQueueClear failed. error = %s" % GigECamera.errstr(ret))

    def PvAttrList(self):
        """ List all camera and driver attributes. """
        logging.info("")
        attrib = c_char_p(0)
        attrListPtr = pointer(attrib)
        len = c_int(0)
        GigECamera.api.PvAttrList.argtypes = [ c_char_p, POINTER(POINTER(c_char_p)), POINTER(c_int) ]
        ret = GigECamera.api.PvAttrList(self.camHandle, byref(attrListPtr), byref(len))
        logging.info("")
        attributes = []
        if ret:
            logging.error("error code = %s" % GigECamera.errstr(ret))
            raise Exception("PVAttrList failed. error = %s" % GigECamera.errstr(ret))
        else:
            for i in range(len.value):
                # print attrListPtr[i]
                attributes.append(attrListPtr[i])
        return attributes

    def PvAttrInfo(self, name):
        """
        Get information, such as data type and access mode, on a particular attribute.
        """
        info = AttributeInfo()
        ret = GigECamera.api.PvAttrInfo(self.camHandle, name, pointer(info))
        if ret:
            logging.error("error code = %s" % GigECamera.errstr(ret))
            raise Exception("PVAttrInfo(%s) failed. error = %s" % (name, GigECamera.errstr(ret)))
        logging.info("%s data type = %d" % (name, info.dataType))
        return info

    def PvAttrEnumGet(self, name):
        """ Get the value of an enumeration attribute. """
        buf = c_char_p('0' * 100)
        bufSize = c_ulong(100)
        size = c_ulong(0)
        ret = GigECamera.api.PvAttrEnumGet(self.camHandle, name, buf, bufSize, byref(size))
        if ret:
            logging.error("error code = %s" % GigECamera.errstr(ret))
            raise Exception("PVAttrEnumGet(%s) failed. error = %s" % (name, GigECamera.errstr(ret)))
        logging.info("%s = %s" % (name, buf.value))
        return buf.value

    def PvAttrEnumSet(self, name, value):
        """ Set the value of an enumeration attribute. """
        logging.info("%s = %s" % (name, value))
        ret = GigECamera.api.PvAttrEnumSet(self.camHandle, name, value)
        if ret:
            logging.error("error code = %s" % GigECamera.errstr(ret))
            raise Exception("PVAttrEnumSet(%s=%s) failed. error = %s" % (name, value, GigECamera.errstr(ret)))

    def PvAttrFloat32Get(self, name):
        """ Get the value of a Float32 attribute. """
        val = c_float(0)
        ret = GigECamera.api.PvAttrFloat32Get(self.camHandle, name, byref(val))
        if ret:
            logging.error("error code = %s" % GigECamera.errstr(ret))
            raise Exception("PVAttrFloat32Get(%s) failed. error = %s" % (name, GigECamera.errstr(ret)))
        logging.error("%s = %f" % (name, float(val.value)))
        return float(val.value)

    def PvAttrStringGet(self, name):
        """ Get the value of a string attribute. """
        buf = c_char_p('0' * 100)
        bufSize = c_ulong(100)
        size = c_ulong(0)
        ret = GigECamera.api.PvAttrStringGet(self.camHandle, name, buf, bufSize, byref(size))
        if ret:
            logging.error("error code = %s" % GigECamera.errstr(ret))
            raise Exception("PVAttrStringGet(%s) failed. error = %s" % (name, GigECamera.errstr(ret)))
        logging.info("%s = %s" % (name, buf.value))
        return buf.value

    def PvAttrStringSet(self, name, value):
        """ Set the value of a string attribute. """
        logging.info("%s = %s" % (name, value))
        ret = GigECamera.api.PvAttrStringSet(self.camHandle, name, value)
        if ret:
            logging.error("error code = %s" % GigECamera.errstr(ret))
            raise Exception("PVAttrStringSet(%s=%s) failed. error = %s" % (name, value, GigECamera.errstr(ret)))

    def PvAttrUint32Get(self, name):
        """ Get the value of a Uint32 attribute. """
        val = c_ulong(0)
        ret = GigECamera.api.PvAttrUint32Get(self.camHandle, name, byref(val))
        if ret:
            logging.error("error code = %s" % GigECamera.errstr(ret))
            raise Exception("PVAttrUint32Get(%s) failed. error = %s" % (name, GigECamera.errstr(ret)))
        logging.info("%s = %s" % (name, int(val.value)))
        return int(val.value)

    def PvCommandRun(self, name):
        """
        Run a command. A command is a "valueless" attribute, which executes
        a function when written.
        """
        logging.info("Command = %s" % name)
        ret = GigECamera.api.PvCommandRun(self.camHandle, name)
        if ret:
            logging.error("error code = %s" % GigECamera.errstr(ret))
            raise Exception("PVCommandRun(%s) failed. error = %s" % (name, GigECamera.errstr(ret)))


    def connect(self, ip):
        self.PvVersion()
        self.PvInitializeNoDiscovery()
        self.initialized = True
        self.PvCameraOpenByAddr(ip)
        self.opened = True
        self.PvAttrStringGet('DeviceVendorName')
        self.PvAttrStringGet('ModelName')

        self.width = self.PvAttrUint32Get('Width')
        self.height = self.PvAttrUint32Get('Height')
        self.bytes_per_frame = self.PvAttrUint32Get('TotalBytesPerFrame')
        self.img = np.empty(self.bytes_per_frame+self.bytes_per_frame/3, dtype=np.uint8)

        # self.PvAttrEnumGet('AcquisitionMode') # Single, Multiple, Continuous
        # self.PvAttrEnumGet('FrameStartTriggerMode') # Freerun, FixedRate, Software
        self.PvAttrEnumSet('AcquisitionMode', 'Continuous')
        self.PvAttrEnumSet('FrameStartTriggerMode', 'Freerun')

        self.PvCaptureStart()
        self.capturing = True
        self.PvCaptureQueueFrames()
        self.framesQueued = True


    def disconnect(self):
        if self.framesQueued:
            self.PvCaptureQueueClear()
            self.framesQueued = False
        if self.capturing:
            self.PvCaptureEnd()
            self.capturing = False
        if self.opened:
            self.PvCameraClose()
            self.opened = False
        if self.initialized:
            self.PvUnInitialize()
            self.initialized = False

    def start(self):
        self.PvCommandRun('AcquisitionStart')
        self.running = True
        self.start = time.clock()
        # print 'acquiring ....'

    def stop(self):
        if self.running:
            self.running = False
            self.PvCommandRun('AcquisitionStop')
        t = time.clock()
        logging.info("acquired %d frames in at %.1f fps", self.frames_count,
                     float(self.frames_count) / (t - self.start) )

    def expTime(self):
        return self.PvAttrUint32Get('ExposureValue')

    def gain(self):
        return self.PvAttrUint32Get('GainValue')

    def triggerDelay(self):
        return self.PvAttrUint32Get('FrameStartTriggerDelay')


def main():
    c = GigECamera(None)
    try:
        c.connect('192.168.100.221')
        c.start()

        c.expTime()
        c.gain()
        c.triggerDelay()

        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        c.stop()
        c.disconnect()
        print '^C'
    except Exception, ex:
        print ex

if __name__ == '__main__':
    logging.basicConfig(\
        format='%(filename)s:%(lineno)d:%(levelname)-8s %(funcName)s: %(message)s',
        level=logging.DEBUG)   # DEBUG, INFO, ERROR, CRITICAL
    main()
