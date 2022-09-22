#!/usr/bin/python
import os
from pydub import AudioSegment
from pydub.silence import split_on_silence
count = 0
def remove_silence(path1,path2):
    for root, dirs, files in os.walk(path1):
        for name in files:
            xx(os.path.join(root, name),name,path2)    
 
def xx(file1,name,path2):
    global count
    count += 1
    print("%d: %s\n"%(count, file1))
    sound = AudioSegment.from_mp3(file1)       
    chunks = split_on_silence(sound,
        # must be silent for at least half a second,沉默半秒
        min_silence_len=1000,     
        # consider it silent if quieter than -16 dBFS        
        silence_thresh=-60,
        keep_silence=650 
    )
    sum=sound[:1]    
    for i, chunk in enumerate(chunks):        
        sum=sum+chunk        
    sum.export(os.path.join(path2, name), format="mp3")
    print(name+"  已完成。") 
    
remove_silence("./wn3", "./dwn3")
