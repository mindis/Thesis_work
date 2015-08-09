data {
  // This version has a generated quantities block to generate estimates of the permanent component.
  int T; // number of observations
  int P; // number of variables
  vector[P] Y[T]; // An array of P columns and T rows containing the data
  vector[P*P] linear_est_1; // prior for theta_1
  vector[P*P] linear_est_2; // prior for theta_2
  vector[P] linear_int; // prior for the intercept
  vector[T] weights; // The analogy weights
  real nu_var; // The degrees of freedom paramter for the LKJ prior on the residual correlation matrix
  }
parameters {
  vector[P] intercept;
  matrix[P,P] theta_1; 
  matrix[P,P] theta_2; 
  corr_matrix[P] Omega_VAR;
  vector<lower= 0>[P] tau_VAR;
}
transformed parameters{
  matrix[P,P] Sig_VAR;
  Sig_VAR <- quad_form_diag(Omega_VAR, tau_VAR);
}
model {
  // LKJ priors for the covariance matrices
  tau_VAR ~ cauchy(0,2.5);
  Omega_VAR ~ lkj_corr(nu_var);
  
  // prior for the regression coeffiecients
  //intercept ~ multi_normal(linear_int, Sig_int);
  intercept ~ normal(0, 1);
  to_vector(theta_1) ~ normal(linear_est_1, 0.3);
  to_vector(theta_2) ~ normal(linear_est_2, 0.3);
  
  for(t in 3:T){
    increment_log_prob(weights[t]*multi_normal_log(Y[t], intercept + theta_1*Y[t-1] + theta_2*Y[t-2], Sig_VAR));
  }
}
generated quantities{
  vector[P] Yhat[T]; // The series that will contain the permanent components
  
  for(t1 in 1:2){
    Yhat[t1] <- Y[t1];
  }
  for(t in 3:T){
    Yhat[t] <- multi_normal_rng(intercept + theta_1*Y[t-1] + theta_2*Y[t-2], Sig_VAR);
  }
}
