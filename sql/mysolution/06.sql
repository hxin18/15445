select a.count*100.0/ count(cases.case_id) from (select count(case_id) as count  from cases where status like '%Case Closed Statistically%') as a , cases;

