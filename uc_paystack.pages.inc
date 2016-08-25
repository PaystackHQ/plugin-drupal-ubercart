<?php

function uc_paystack_complete($cart_id = 0) {
  die('dddddd');
  $order_id = check_plain($_POST['Param1']);
  $payment_status = check_plain($_POST['Result']);
  $payment_amount = check_plain($_POST['Charge']);
  $payment_currency = check_plain($_POST['Currency']);
  $ErrorMessage = check_plain($_POST['ErrorMessage']);
}
