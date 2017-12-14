select cases.violation_county, count(cases.case_id) as count 
 from cases, charges
 where cases.case_id = charges.case_id
 and cases.violation_county <> ''
 and charges.description like '%RECKLESS%'
 group by cases.violation_county
 order by count desc, cases.violation_county asc limit 3;

