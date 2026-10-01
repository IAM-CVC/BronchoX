
%% Load and Skel 
skel = flipdim(skel,3);

%% afegim una poteta al final de la traquea per tenir el primer node
w=size(skel,1);
l=size(skel,2);
h=size(skel,3);
[x,y,z]=ind2sub([w,l,h],find(skel(:)));
zTraq=1:min(z);
xTraq=x(1)*ones(size(zTraq));
yTraq=y(1)*ones(size(zTraq))+1;
indTraq=sub2ind(size(skel),xTraq,yTraq,zTraq);
skel(indTraq)=1;


[ link,skelP,node,A ] = Skel_BranchPoints_Deb(skel,0);

%% Graph structure

G = GraphAdapt(node,link,A);

%% Remove middle nodes
GPrimerPas = DelMiddleNode(G);
GPrimerPas = CompactGraph(GPrimerPas); %per si perdem el root

%% Clique/clique Detection and treatment

%GSegonPas=CyclesTract(GPrimerPas);
GSegonPas=Cycle3nodesTract(GPrimerPas);
GSegonPas = CompactGraph(GSegonPas);

%% End points prunning

GTercerPas = EndPointPruning(GSegonPas);

%% Remove middle nodes and get final directed graph

GFinal = DelMiddleNode(GTercerPas);


GFinal = CompactGraph(GFinal);
GFinalDir = GFinal;
GFinalDir.e = Graph2dirGraph(GFinal.e);

%Girem Z dels nodes
GFinal.v(:,3) = FlipZDim(skel, GFinal.v(:,3));
%Girem X,Y els nodes i els desplacem a la ROI
v_temp= GFinal.v;
GFinal.v(:,1) = v_temp(:,2);
GFinal.v(:,2) = v_temp(:,1);

%reconstruim el skel aplicant XYZUniform i fent el canvi de x,y i el flip
%en z
GFinal = FlipSkel(GFinal,skel,ROI);

GFinal.v(:,1)=GFinal.v(:,1)+ROI.j(1)-1;
GFinal.v(:,2)=GFinal.v(:,2)+ROI.i(1)-1;
GFinal.v(:,3)=GFinal.v(:,3)+ROI.k(1)-1;


GVal = GFinal;

save([ OutPutDataFolder filesep 'GVal'],'GVal')
