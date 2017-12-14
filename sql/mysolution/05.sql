 select count(case_id) as count,substr(filing_date,1,3) || '0s' as decade from cases 
   where decade <> '0s' 
   group by decade   
   order by count desc, decade asc
   limit 3;

