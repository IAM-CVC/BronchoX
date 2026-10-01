function [ GSub ] = SegBranchLocalComplexity_26_10_2018( skelDistal,varargin )


% per calcular la complexitat no cal identificar el punt d'entrada. A mes
% la identificació d'aquest punt tal i com està feta introdueix complexitat
% falsa en alguns casos

[ Adj,node,link] = Skel2Graph3D_Deb_07_06_2017(skelDistal,0);

GSub = GraphAdapt(node,link,Adj);
% Define root as closest point to main airways
rootNode=1;
if(~isempty(varargin))
    [rootNode]=BranchRoot(GSub,varargin{1});
end
GSub = ComplexityPropsDeb(GSub,'Leaf',rootNode);
GSub.rootNode=rootNode;

end

