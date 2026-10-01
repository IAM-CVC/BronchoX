
SegParamSettings;
MainBronchiEnerParams.GPUUSe='Half';

if ~exist(OutPutDataFolder, 'dir')
   mkdir(OutPutDataFolder)
end

load([ DataFolder filesep 'CTData'])
