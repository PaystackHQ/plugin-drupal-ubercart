<?php

function uc_paystack_complete($cart_id = 0) {
  die('dddddd');
  $order_id = check_plain($_POST['Param1']);
  $payment_status = check_plain($_POST['Result']);
  $payment_amount = check_plain($_POST['Charge']);
  $payment_currency = check_plain($_POST['Currency']);
  $ErrorMessage = check_plain($_POST['ErrorMessage']);
}
// function my_pay_gateway_complete() {
//
//   if (empty($_POST)) {
//     watchdog('My Payment Gateway',
//              'Received an empty or incomplete response.  Response details: @request_details',
//              array('@request_details' =>  print_r($_POST,true)), WATCHDOG_ERROR);
//
//     return 'There was a problem with your payment';
//   }
//
//   if ($_POST['status'] == 'SUCCESS') {
//
//     // Insert logic here to make sure payment info can be matched to valid order
//
//     // Assuming all tests passed and payment was successful
//     // Complete the order
//     uc_payment_enter($order_id, 'my_pay_gateway', $amount, $order->uid, NULL, $orderId);
//     uc_cart_complete_sale($order, variable_get('uc_new_customer_login', FALSE));
//
//     return 'Thank you for your purchase';
//   }
// }
public function verify_txn($code){
      // $test_secretkey = Configuration::get('PAYSTACK_TEST_SECRETKEY');
      // $live_secretkey = Configuration::get('PAYSTACK_LIVE_SECRETKEY');
      // $mode = Configuration::get('PAYSTACK_MODE');
      //
      // if ($mode == 'test') {
      //   $key = $test_secretkey;
      // }else{
      //   $key = $live_secretkey;
      // }
      // $contextOptions = array(
      //     'ssl' => array(
      //         'verify_peer' => true,
      //         // 'cafile' => '/Applications/AMPPS/php-7.0/etc/cacert.pem',
      //         'ciphers' => 'HIGH:!SSLv2:!SSLv3',
      //     ),
      //     'http'=>array(
     // 		    'method'=>"GET",
      //       'header'=> array("Authorization: Bearer ".$key."\r\n","Connection: close\r\n","User-Agent: test\r\n)")
     // 		  )
      // );
      //
      $context = stream_context_create($contextOptions);
      $url = 'https://api.paystack.co/transaction/verify/'.$code;
      $request = Tools::file_get_contents($url, false, $context);
      $result = Tools::jsonDecode($request);
      $result = json_decode('{
          "status": true,
          "message": "Verification successful",
          "data": {
            "amount": 54000,
            "transaction_date": "2016-08-19T14:16:44.000Z",
            "status": "success",
            "reference": "'.$code.'",
            "domain": "test",
            "authorization": {
              "authorization_code": "AUTH_2mnfo76b",
              "card_type": "visa",
              "last4": "1381",
              "exp_month": "01",
              "exp_year": "2020",
              "bank": "TEST BANK",
              "channel": "card",
              "reusable": true
            },
            "customer": {
              "first_name": "",
              "last_name": "",
              "email": "kendyson@kendyson.com"
            },
            "plan": null
          }
        }');

      // $result = json_decode('{
      //     "status": false,
      //     "message": "Invalid transaction reference"
      //   }');
      return $result;
    }
