with cr as
(select c.case_id as id, c.filing_date as date 
from cases as c, charges 
where charges.case_id = c.case_id 
and charges.disposition='Guilty'
)
select strftime('%Y',cr.date),avg(-strftime('%Y.%m%d',p.dob)+strftime('%Y.%m%d',cr.date)) as avgg from parties as p, cr
where p.case_id = cr.id
 and p.dob is not null
 and p.name <> ''
and p.type = 'Defendant'
 and cr.date <> ''
and -strftime('%Y.%m%d',p.dob)+strftime('%Y.%m%d',cr.date) >0
and -strftime('%Y.%m%d',p.dob)+strftime('%Y.%m%d',cr.date) <100
 group by strftime('%Y',cr.date)
 order by strftime('%Y',cr.date) desc
limit 5;


