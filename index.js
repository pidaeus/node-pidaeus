module.exports = process.env.pidaeus_COV
  ? require('./lib-cov/pidaeus')
  : require('./lib/pidaeus');
