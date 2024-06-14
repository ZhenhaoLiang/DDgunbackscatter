# coding=utf-8
import numpy as np
import pandas as pd
import glob
import uproot
import tqdm
import awkward as ak

def tracks_from_file(tr_file, tr_ttree='event',
                     step_vals=None):
    tr_columns = step_vals
    print(f'uproot.open : {tr_file} . . . ')
    T = uproot.open(tr_file)
    if ('event;1' in T.keys()) == False:
        return 0
    ttree = uproot.open(tr_file)[tr_ttree]
    events = ttree.arrays(tr_columns)

    run_data = []
    for i in range(0,len(events)):
        temp = []
        for column in step_vals:
            temp.append(events[i][column])
            run_data.append(temp)
    dt = pd.DataFrame(run_data, columns=step_vals, dtype=object)
    dt = dt.dropna(axis=0) #删除含有缺失值的行

    return dt

file_list = glob.glob(f"out/**.root")  #root文件路径
#event_vals = ['eventid', ]
step_vals = ['Energy','prex', 'prey', 'prez','postx',    #要读取的信息。
            'posty', 'postz', 'ptype', 'eventID',
            'trackID','parentID', 'dE']
df = pd.DataFrame()
count = 0
for ind, f in tqdm.tqdm(enumerate(file_list)):
    _df = tracks_from_file(tr_file=f,
                           tr_ttree='event',
                           step_vals=step_vals)
    if isinstance(_df,pd.DataFrame):
        _df.eventID += count * 10000000
        count = count + 1
        df = pd.concat((df, _df), ignore_index=True)

df.loc[:, 'step'] = 1
for i in range(1,len(df)):
    if (df.loc[i].ptype == df.loc[i-1].ptype) & (df.loc[i].eventID == df.loc[i-1].eventID) &(df.loc[i].trackID == df.loc[i-1].trackID):
        df.loc[i,'step'] = df.loc[i-1,'step'] + 1
df.to_csv("out/merge.csv", index=False)