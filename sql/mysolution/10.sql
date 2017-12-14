select p.zip,count(p.case_id) 
from parties as p, charges as c
where p.case_id =c.case_id 
and p.zip <> ''
group by p.zip
order by count(p.case_id) desc
limit 3
;

