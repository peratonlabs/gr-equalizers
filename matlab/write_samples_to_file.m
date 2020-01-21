% /* -*- c++ -*- */
% /*
%  * Copyright 2019, 2020 Perspecta Labs Inc.
%  *
%  * This is free software; you can redistribute it and/or modify
%  * it under the terms of the GNU General Public License as published by
%  * the Free Software Foundation; either version 3, or (at your option)
%  * any later version.
%  *
%  * This software is distributed in the hope that it will be useful,
%  * but WITHOUT ANY WARRANTY; without even the implied warranty of
%  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%  * GNU General Public License for more details.
%  *
%  * You should have received a copy of the GNU General Public License
%  * along with this software; see the file COPYING.  If not, write to
%  * the Free Software Foundation, Inc., 51 Franklin Street,
%  * Boston, MA 02110-1301, USA.
%  */

function write_samples_to_file(tx_wf, filename)
%write_samples_to_file - writes waveform samples to file(s)

L = size(tx_wf,2);

for n = 1:L
   if (L > 1)
     filename_wr = [filename  '_' num2str(n-1) '.dat'];
   else
     filename_wr = [filename];
   end
   
   fid = fopen(filename_wr,'w');
   
   interleaved = [real(tx_wf(:,n)).';imag(tx_wf(:,n)).'];
   fwrite(fid,interleaved,'float32');    
    
end
