import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import  ConnectionPatch
import warnings
warnings.filterwarnings('ignore')
from matplotlib import pyplot as plt
from matplotlib import ticker
from matplotlib.ticker import FuncFormatter
import random
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt(open("E:\c++\paper\ConsoleApplication1\\small_FE.csv","rb"),delimiter=",",skiprows=0) 
data=data.T
plt.figure(dpi=800,figsize=(8,8))
l=len(data[0][:])
x=np.arange(0, l)
line1,=plt.plot(x,data[0][:],color = '#FA7F6F', marker='1',markeredgewidth='2',linewidth = '1')#ILP下
line2,=plt.plot(x,data[1][:],color = '#32B897',marker='2',markeredgewidth='2',linewidth = '1')#ILP上界
line3,=plt.plot(x,data[2][:],color = '#6699FF',marker='o',markerfacecolor='none', markeredgewidth='1.5',linewidth = '1')#J-RSAFE
line4,=plt.plot(x,data[3][:],color = '#FFBE7A',marker='x',markeredgewidth='2',linewidth = '1')#WOH
line5,=plt.plot(x,data[4][:],color = '#BEB8DC',marker='+', markeredgewidth='2',linewidth = '1')#BLSA
line6,=plt.plot(x,data[5][:],color = '#82B0D2',marker='^',markerfacecolor='none',markeredgewidth='1.5',linewidth = '1')#WOH
line7,=plt.plot(x,data[6][:],color = '#999999',marker='d',markerfacecolor='none', markeredgewidth='1.5',linewidth = '1')#BLSA
ll=plt.legend([line1,line2,line3,line4,line5,line6,line7],["ILP_LB","ILP_UB", "J-RSAFE","WOH-Npo","BLSA","M-BLSA","ACF-CA"],loc='upper left',fontsize=12)#添加图例
# 定义下标数字的映射表
sub_map = str.maketrans('0123456789,', '₀₁₂₃₄₅₆₇₈₉,')
index_ls=['NSF1,2,2'.translate(sub_map),'NSF2,2,2'.translate(sub_map),'NSF2,2,3'.translate(sub_map),'NSF1,3,3'.translate(sub_map),'NSF2,3,3'.translate(sub_map),'NSF1,2,3'.translate(sub_map),    'US1,2,2'.translate(sub_map),'US2,2,2'.translate(sub_map),'US2,2,3'.translate(sub_map),'US1,3,3'.translate(sub_map),'US2,3,3'.translate(sub_map),'US1,2,3'.translate(sub_map),    'Japan1,2,2'.translate(sub_map),'Japan2,2,2'.translate(sub_map),'Japan2,2,3'.translate(sub_map),'Japan1,3,3'.translate(sub_map),'Japan2,3,3'.translate(sub_map),'Japan1,2,3'.translate(sub_map),  'Spanish1,2,2'.translate(sub_map),'Spanish2,2,2'.translate(sub_map),'Spanish2,2,3'.translate(sub_map),'Spanish1,3,3'.translate(sub_map),'Spanish2,3,3'.translate(sub_map),'Spanish1,2,3'.translate(sub_map),]
plt.xticks(x,index_ls,fontsize=9,rotation=35, ha='right') 
plt.ylabel("The number of fiber expansions",fontsize=14)#设置纵轴单位
plt.xlabel("Problem instances",fontsize=14)#设置横轴单位
plt.savefig('small_FE.png')


data = np.loadtxt(open("E:\c++\paper\ConsoleApplication1\\large_FE.csv","rb"),delimiter=",",skiprows=0) 
data=data.T
plt.figure(dpi=800,figsize=(12,12))
l=len(data[0][:])
x=np.arange(0, l)
line3,=plt.plot(x,data[0][:],color = '#6699FF',marker='o',markerfacecolor='none', markeredgewidth='1.5',linewidth = '1')#J-RSAFE
line4,=plt.plot(x,data[1][:],color = '#FFBE7A',marker='x',markeredgewidth='2',linewidth = '1')#WOH
line5,=plt.plot(x,data[2][:],color = '#BEB8DC',marker='+', markeredgewidth='2',linewidth = '1')#BLSA
line6,=plt.plot(x,data[3][:],color = '#82B0D2',marker='^',markerfacecolor='none',markeredgewidth='1.5',linewidth = '1')#WOH
line7,=plt.plot(x,data[4][:],color = '#999999',marker='d',markerfacecolor='none', markeredgewidth='1.5',linewidth = '1')#BLSA

ll=plt.legend([line3,line4,line5,line6,line7],["J-RSAFE","WOH-Npo","BLSA","M-BLSA","ACF-CA"],loc='upper left',fontsize=13)#添加图例
# 定义下标数字的映射表
sub_map = str.maketrans('0123456789,', '₀₁₂₃₄₅₆₇₈₉,')
index_ls=['enron-only1,64,128'.translate(sub_map),'enron-only1,128,128'.translate(sub_map),'enron-only2,64,128'.translate(sub_map),'enron-only2,128,128'.translate(sub_map),'enron-only3,64,128'.translate(sub_map),'enron-only3,128,128'.translate(sub_map),'infect-dublin1,64,128'.translate(sub_map),'infect-dublin1,128,128'.translate(sub_map),'infect-dublin2,64,128'.translate(sub_map),'infect-dublin2,128,128'.translate(sub_map),'infect-dublin3,64,128'.translate(sub_map),'infect-dublin3,128,128'.translate(sub_map),'crime-moreno1,64,128'.translate(sub_map),'crime-moreno1,128,128'.translate(sub_map),'crime-moreno2,64,128'.translate(sub_map),'crime-moreno2,128,128'.translate(sub_map),'crime-moreno3,64,128'.translate(sub_map),'crime-moreno3,128,128'.translate(sub_map),'email-univ1,64,128'.translate(sub_map),'email-univ1,128,128'.translate(sub_map),'email-univ2,64,128'.translate(sub_map),'email-univ2,128,128'.translate(sub_map),'email-univ3,64,128'.translate(sub_map),'email-univ3,128,128'.translate(sub_map)]

plt.xticks(x,index_ls,fontsize=9,rotation=35, ha='right') 
plt.ylabel("The number of fiber expansions",fontsize=15)#设置纵轴单位
plt.xlabel("Problem instances",fontsize=15)#设置横轴单位
plt.savefig('large_FE.png')


data = np.loadtxt(open("E:\c++\paper\ConsoleApplication1\\small_ratio.csv","rb"),delimiter=",",skiprows=0,usecols=(1,2,3,4,5,6)) 
data=data.T
plt.figure(dpi=800,figsize=(8,8))
l=len(data[0][:])
x=np.arange(0, l)
line1,=plt.plot(x,data[0][:],color = '#FA7F6F', marker='1',markeredgewidth='2',linewidth = '1')#ILP下
line3,=plt.plot(x,data[1][:],color = '#6699FF',marker='o',markerfacecolor='none', markeredgewidth='1.5',linewidth = '1')#J-RSAFE
line4,=plt.plot(x,data[2][:],color = '#FFBE7A',marker='x',markeredgewidth='2',linewidth = '1')#WOH
line5,=plt.plot(x,data[3][:],color = '#BEB8DC',marker='+', markeredgewidth='2',linewidth = '1')#BLSA
line6,=plt.plot(x,data[4][:],color = '#82B0D2',marker='^',markerfacecolor='none',markeredgewidth='1.5',linewidth = '1')#WOH
line7,=plt.plot(x,data[5][:],color = '#999999',marker='d',markerfacecolor='none', markeredgewidth='1.5',linewidth = '1')#BLSA

ll=plt.legend([line1,line3,line4,line5,line6,line7],["ILP","J-RSAFE","WOH-Npo","BLSA","M-BLSA","ACF-CA"],loc='upper left',fontsize=12)#添加图例
# 定义下标数字的映射表
sub_map = str.maketrans('0123456789,', '₀₁₂₃₄₅₆₇₈₉,')
index_ls=['NSF1,2,2'.translate(sub_map),'NSF1,3,3'.translate(sub_map),'NSF2,3,3'.translate(sub_map),'NSF1,2,3'.translate(sub_map),    'US1,2,2'.translate(sub_map),'US1,3,3'.translate(sub_map),'US1,2,3'.translate(sub_map),    'Japan1,2,2'.translate(sub_map),'Japan2,2,2'.translate(sub_map),'Japan1,3,3'.translate(sub_map),'Japan1,2,3'.translate(sub_map),  'Spanish1,2,2'.translate(sub_map),'Spanish1,3,3'.translate(sub_map),'Spanish1,2,3'.translate(sub_map)]
plt.xticks(x,index_ls,fontsize=9,rotation=35, ha='right') 
plt.ylabel("The ratio",fontsize=14)#设置纵轴单位
plt.xlabel("Problem instances",fontsize=14)#设置横轴单位
plt.savefig('small_ratio.png')

data = np.loadtxt(open("E:\c++\paper\ConsoleApplication1\\large_ratio.csv","rb"),delimiter=",",skiprows=0,usecols=(1,2,3,4,5)) 
data=data.T
plt.figure(dpi=800,figsize=(12,12))
l=len(data[0][:])
x=np.arange(0, l)
line3,=plt.plot(x,data[0][:],color = '#6699FF',marker='o',markerfacecolor='none', markeredgewidth='1.5',linewidth = '1')#J-RSAFE
line4,=plt.plot(x,data[1][:],color = '#FFBE7A',marker='x',markeredgewidth='2',linewidth = '1')#WOH
line5,=plt.plot(x,data[2][:],color = '#BEB8DC',marker='+', markeredgewidth='2',linewidth = '1')#BLSA
line6,=plt.plot(x,data[3][:],color = '#82B0D2',marker='^',markerfacecolor='none',markeredgewidth='1.5',linewidth = '1')#WOH
line7,=plt.plot(x,data[4][:],color = '#999999',marker='d',markerfacecolor='none' ,markeredgewidth='1.5',linewidth = '1')#BLSA

ll=plt.legend([line3,line4,line5,line6,line7],["J-RSAFE","WOH-Npo","BLSA","M-BLSA","ACF-CA"],loc='upper left',fontsize=13)#添加图例
# 定义下标数字的映射表
sub_map = str.maketrans('0123456789,', '₀₁₂₃₄₅₆₇₈₉,')
index_ls=['enron-only1,64,128'.translate(sub_map),'enron-only1,128,128'.translate(sub_map),'enron-only2,64,128'.translate(sub_map),'enron-only2,128,128'.translate(sub_map),'enron-only3,64,128'.translate(sub_map),'enron-only3,128,128'.translate(sub_map),'infect-dublin1,64,128'.translate(sub_map),'infect-dublin1,128,128'.translate(sub_map),'infect-dublin2,64,128'.translate(sub_map),'infect-dublin2,128,128'.translate(sub_map),'infect-dublin3,64,128'.translate(sub_map),'infect-dublin3,128,128'.translate(sub_map),'crime-moreno1,64,128'.translate(sub_map),'crime-moreno1,128,128'.translate(sub_map),'crime-moreno2,64,128'.translate(sub_map),'crime-moreno2,128,128'.translate(sub_map),'crime-moreno3,64,128'.translate(sub_map),'crime-moreno3,128,128'.translate(sub_map),'email-univ1,64,128'.translate(sub_map),'email-univ1,128,128'.translate(sub_map),'email-univ2,64,128'.translate(sub_map),'email-univ2,128,128'.translate(sub_map),'email-univ3,64,128'.translate(sub_map),'email-univ3,128,128'.translate(sub_map)]

plt.xticks(x,index_ls,fontsize=9,rotation=35, ha='right') 
plt.ylabel("The ratio",fontsize=15)#设置纵轴单位
plt.xlabel("Problem instances",fontsize=15)#设置横轴单位
plt.savefig('large_ratio.png')