<?php
function initParams(){
    $params = [];
    $transaction = array();
    $nbProducts = $this->context->cart->nbProducts();//self::$cart->nbProducts();
    $this->context->smarty->assign('nb_products', $nbProducts);

    if($nbProducts  > 0 && Tools::getValue('txn_code') !== ''){
      $txn_code = Tools::getValue('txn_code');
      $amount = Tools::getValue('amounttotal');
      $email = Tools::getValue('email');
      $verification = $this->verify_txn($txn_code);

      $paystack = new PrestaPaystack();
      if(($verification->status===false) || (!property_exists($verification, 'data')) || ($verification->data->status !== 'success')){
        // request to paystack failed
        $date = date("Y-m-d h:i:sa");
        $email = $email;
        $total = $amount;
        // $verification->message;
        $status = 'failed';
      } else {
        $email = $verification->data->customer->email;
        $date = $verification->data->transaction_date;
        $total = $verification->data->amount/100;
        $status = 'approved';
      }

      $transaction_id = $txn_code;

      if (trim(Tools::strtolower($status)) == 'approved'){
        $params = array(
          array('value' => $email, 'name' => 'Email'),
          array('value' => $total, 'name' => 'Total'),
          array('value' => $date, 'name' => 'Date'),
          array('value' => $transaction_id, 'name' => 'Transaction ID'),
          array('value' => $status, 'name' => 'Status'),
        );
        $paystack->validation($verification);
      }else{
        $params = array(
          array('value' => $email, 'name' => 'Email'),
          array('value' => $total, 'name' => 'Total'),
          array('value' => $date, 'name' => 'Date'),
          array('value' => $transaction_id, 'name' => 'Transaction ID'),
          array('value' => $status, 'name' => 'Status'),
        );
        $paystack->validation($verification);

      }
      $this->context->smarty->assign('status', $status);
      $return_url = __PS_BASE_URI__.'order-history';
      $this->context->smarty->assign('return_url', $return_url);

    }
    return $params;
  }
function uc_paystack_complete($cart_id = 0) {
  // die('dddddd');
  $order_id = check_plain($_POST['order_id']);
  $code = check_plain($_POST['code']);
  $payment_amount = check_plain($_POST['order_total']);
  function verify_txn($code){
        // $test_secretkey = Configuration::get('PAYSTACK_TEST_SECRETKEY');
        // $live_secretkey = Configuration::get('PAYSTACK_LIVE_SECRETKEY');
        // $mode = Configuration::get('PAYSTACK_MODE');
        //
        $key = variable_get ('test_secretkey', '');
        // die($key.'ssss');
        // if ($mode == 'test') {
        //   $key = $test_secretkey;
        // }else{
        //   $key = $live_secretkey;
        // }
        $url = 'https://api.paystack.co/transaction/verify/'.$code;
        $options = array(
          'method' => 'GET', // HTTP Request Type
          'headers' => array('Authorization' => 'Bearer '.$key),
        );
        $request = drupal_http_request($url, $options);
        $result = json_decode($request->data);
        // print_r($result);

        // print_r($result);
        // $result = json_decode('{
        //     "status": true,
        //     "message": "Verification successful",
        //     "data": {
        //       "amount": 54000,
        //       "transaction_date": "2016-08-19T14:16:44.000Z",
        //       "status": "success",
        //       "reference": "'.$code.'",
        //       "domain": "test",
        //       "authorization": {
        //         "authorization_code": "AUTH_2mnfo76b",
        //         "card_type": "visa",
        //         "last4": "1381",
        //         "exp_month": "01",
        //         "exp_year": "2020",
        //         "bank": "TEST BANK",
        //         "channel": "card",
        //         "reusable": true
        //       },
        //       "customer": {
        //         "first_name": "",
        //         "last_name": "",
        //         "email": "kendyson@kendyson.com"
        //       },
        //       "plan": null
        //     }
        //   }');

        // $result = json_decode('{
        //     "status": false,
        //     "message": "Invalid transaction reference"
        //   }');
        return $result;
  }

  $verification = verify_txn($code);


  $order = uc_order_load($order_id);
  $debugmessage = "";
  if(($verification->status===false) || (!property_exists($verification, 'data')) || ($verification->data->status !== 'success')){
    uc_order_comment_save($order->order_id, 0, t("The customer cancelled payment."), 'order', 'canceled' );
    drupal_set_message($debugmessage .t('Your payment was cancelled. Please feel free to continue shopping or contact us for assistance.'));
    unset($_SESSION['cart_order']);
    drupal_goto(variable_get('uc_mypayment_cancel_return_url', 'cart'));
  } else {
    $email = $verification->data->customer->email;
    // $date = $verification->data->transaction_date;
    $payment_amount  = $verification->data->amount/100;
    // $status = 'approved';
    $comment = t('Paystack transaction ID: @PayId , Email: @Email', array('@PayId' => $code,'@Email' => $email));
    uc_payment_enter($order->order_id, 'Paystack', $payment_amount, $order->uid, NULL, $comment);
    uc_cart_complete_sale($order);
    uc_order_comment_save($order->order_id, 0, t('Payment of @amount @currency submitted through Paystack.', array('@amount' =>   $payment_amount , '@currency' => 'NGN')), 'order', 'payment_received');
    uc_order_comment_save($order->order_id, 0, t('Paystack reported a payment of @amount @currency', array('@amount' =>   $payment_amount , '@currency' => 'NGN')));
    drupal_set_message($debugmessage . t('Your payment was completed.'));
    drupal_goto(variable_get('paystack_success_return_url', 'cart'));
  }

  // $message = $debugmessage . t("Your payment failed with following error message: @Error", array('@Error' => $ErrorMessage));
  // uc_order_comment_save($order->order_id, 0, $message, 'admin');
  // drupal_set_message($message . t(' Please try again in a few moments.'));
  // drupal_goto(variable_get('uc_mypayment_error_return_url', 'cart'));
}
