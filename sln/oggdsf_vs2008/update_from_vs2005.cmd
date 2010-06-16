@echo off
copy ..\oggdsf_vs2005\oggdsf.sln oggdsf_2008.sln
sed -i -s "s/Format Version 9.00/Format Version 10.00/"  oggdsf_2008.sln
sed -i -s "s/Visual Studio 2005/Visual Studio 2008/"  oggdsf_2008.sln
sed -i -s "s/-2005.vcproj/.vcproj/g"  oggdsf_2008.sln
sed -i -s "s/VS2005/VS2008/g"  oggdsf_2008.sln
sed -i -s "s/_2005/_2008/g"  oggdsf_2008.sln
sed -i -s "s/vpx.vcproj/vpx_2008.vcproj/g"  oggdsf_2008.sln
del sed*
move oggdsf_2008.sln oggdsf.sln