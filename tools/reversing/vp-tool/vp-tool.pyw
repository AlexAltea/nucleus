#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import subprocess
import wx

# Configuration
BIN_CGCASM = "..."
BIN_CGCDIS = "..."

FILE_CG_SRC = "shader-cg.txt"
FILE_RSX_BIN = "shader-rsx.bin"
FILE_RSX_DIS = "shader-rsx.txt"
FILE_GLSL_SRC = "shader-glsl.txt"

def system(cmd):
    startupinfo = subprocess.STARTUPINFO()
    startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    subprocess.call(cmd, startupinfo=startupinfo)

def GLSL2CG(s):
    # TODO: This GLSL-to-CG translator is probably crap and unreliable
    header = """
    struct v2fConnector
    {
        float4 projCoord : POSITION;
        float4 color     : COLOR0;
        float4 tex       : TEX0;
    };

    struct f2fConnector
    {
      float4 COL0 : COLOR0;
    };
    """
    mainHeader = """
    // Main
    f2fConnector main
    (
        v2fConnector v2f,
        uniform float iGlobalTime,
        uniform float2 iGamePad
    )
    {
	// GLSL variables
	#define WIDTH 720.0
	#define HEIGHT 480.0
	float3 iResolution = float3(WIDTH, HEIGHT, 1.0);
	float4 gl_FragCoord = float4(v2f.tex.x*WIDTH, v2f.tex.y*HEIGHT, 1.0, 0.0);
	float4 gl_FragColor;

        f2fConnector f2f;
    """
    mainFooter = """
        // Return
        f2f.COL0 = gl_FragColor;
        return f2f;
    }
    """

    pos = max(s.find("void main()"), s.find("void main(void)"))
    if pos == -1:
        raise "GLGS2CG: Error: main() not found"

    result = header + s[:pos] + mainHeader
    if "void main()" in s:      result += s[pos+11:]
    if "void main(void)" in s:  result += s[pos+15:]
    result += mainFooter

    result = result.replace("vec2","float2")
    result = result.replace("vec3","float3")
    result = result.replace("vec4","float4")
    result = result.replace("mat2","float2x2")
    result = result.replace("mat3","float3x3")
    result = result.replace("mat4","float4x4")
    result = result.replace("mix","lerp")
    result = result.replace("mod","fmod")
    return result

class Main(wx.Frame):
    def __init__(self, *args, **kwargs):
        super(Main, self).__init__(*args, **kwargs)
        self.InitUI()

    def InitUI(self):
        panel = wx.Panel(self)
        sizer = wx.BoxSizer(wx.VERTICAL)

        #Tools
        sTools = wx.BoxSizer(wx.HORIZONTAL)
        bRun = wx.Button(panel, -1, "Run")
        sTools.AddSpacer(5)
        sTools.Add(bRun)
        sTools.AddSpacer(5)

        wx.StaticText(panel, -1, "Input shader:", pos=(100,10))
        self.rb0 = wx.RadioButton(panel, label="CG", pos=(175,10))
        self.rb1 = wx.RadioButton(panel, label="GLSL", pos=(215,10))
        self.rb0.SetValue(True)
        self.rb1.SetValue(False)

        # First two boxes
        sText0 = wx.BoxSizer(wx.HORIZONTAL)
        self.sb0 = wx.StaticBox(panel, label="1. Original CG Shader")
        sb3 = wx.StaticBox(panel, label="4. Recompiled GLSL Shader")
        sbs0 = wx.StaticBoxSizer(self.sb0, wx.VERTICAL)
        sbs3 = wx.StaticBoxSizer(sb3, wx.VERTICAL)
        self.t0 = wx.TextCtrl(panel, style = wx.TE_MULTILINE)
        self.t3 = wx.TextCtrl(panel, style = wx.TE_MULTILINE)
        sbs0.Add(self.t0, 1, wx.EXPAND)
        sbs3.Add(self.t3, 1, wx.EXPAND)
        sText0.AddSpacer(5)
        sText0.Add(sbs0, 1, wx.EXPAND)
        sText0.AddSpacer(5)
        sText0.Add(sbs3, 1, wx.EXPAND)
        sText0.AddSpacer(5)

        # Last two boxes
        sText1 = wx.BoxSizer(wx.HORIZONTAL)
        sb1 = wx.StaticBox(panel, label="2. RSX Shader (Hex)")
        sb2 = wx.StaticBox(panel, label="3. RSX Shader (Disasm)")
        sbs1 = wx.StaticBoxSizer(sb1, wx.VERTICAL)
        sbs2 = wx.StaticBoxSizer(sb2, wx.VERTICAL)
        self.t1 = wx.TextCtrl(panel, style = wx.TE_MULTILINE)
        self.t1.SetFont(wx.Font(8, wx.FONTFAMILY_MODERN, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL))
        self.t2 = wx.TextCtrl(panel, style = wx.TE_MULTILINE)
        sbs1.Add(self.t1, 1, wx.EXPAND)
        sbs2.Add(self.t2, 1, wx.EXPAND)
        sText1.AddSpacer(5)
        sText1.Add(sbs1, 1, wx.EXPAND)
        sText1.AddSpacer(5)
        sText1.Add(sbs2, 1, wx.EXPAND)
        sText1.AddSpacer(5)

        # Merge everything
        sizer.AddSpacer(5)
        sizer.Add(sTools)
        sizer.AddSpacer(10)
        sizer.Add(sText0, 1, wx.EXPAND)
        sizer.AddSpacer(5)
        sizer.Add(sText1, 1, wx.EXPAND)
        sizer.AddSpacer(5)
        panel.SetSizer(sizer)

        # Events
        self.Bind(wx.EVT_BUTTON, self.OnRun)
        self.rb0.Bind(wx.EVT_RADIOBUTTON,
            lambda e: self.sb0.SetLabel("1. Original CG Shader"))
        self.rb1.Bind(wx.EVT_RADIOBUTTON,
            lambda e: self.sb0.SetLabel("1. Original GLSL Shader (will be translated to CG)"))
            

        # Window
        self.SetSize((880, 520))
        self.SetTitle('RSX VP Recompiler Tool')
        self.Centre()
        self.Show(True)

    def OnRun(self, e):
        # Clear existing files
        files = [
            FILE_CG_SRC,
            FILE_RSX_BIN,
            FILE_RSX_DIS,
            FILE_GLSL_SRC]
        for fname in files:
            try:
                os.remove(fname)
            except OSError:
                pass

        # Get source code, compile it and disassemble it
        if self.rb0.GetValue():
            shaderCode = self.t0.GetValue()
        else:
            shaderCode = GLSL2CG(self.t0.GetValue())
        with open(FILE_CG_SRC, "wb") as f:
            f.write(shaderCode.encode('utf-8'))
        system("%s --profile sce_vp_rsx -o %s %s" %
            (BIN_CGCASM, FILE_RSX_BIN, FILE_CG_SRC))
        if not os.path.isfile(FILE_RSX_BIN):
            wx.MessageDialog(self, "Shader compilation failed!", "Error", wx.ICON_ERROR).ShowModal()
            return
        system("%s -o %s %s" %
            (BIN_CGCDIS, FILE_RSX_DIS, FILE_RSX_BIN))
        if not os.path.isfile(FILE_RSX_DIS):
            wx.MessageDialog(self, "Shader disassembling failed!", "Error", wx.ICON_ERROR).ShowModal()
            return

        # Show output data
        with open(FILE_RSX_DIS, "rb") as f:
            data = f.read()
            self.t2.SetValue(data.decode('utf-8'))
        with open(FILE_RSX_BIN, "rb") as f:
            data = f.read()
        result = ""
        for i in range(len(data) // 16):
            result += "%04x : " % (i * 16)
            for j in range(16):
                result += "%02X " % (data[i * 16 + j])
            result += "\n"
        self.t1.SetValue(result)

if __name__ == '__main__':
    ex = wx.App(0)
    Window = Main(None)
    ex.MainLoop()
