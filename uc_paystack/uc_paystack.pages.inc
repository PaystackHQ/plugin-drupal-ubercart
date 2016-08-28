<?php


function uc_paystack_complete($cart_id = 0) {
  if (empty($_POST) || !isset($_POST['order_id'])) {
    drupal_goto(variable_get('uc_mypayment_cancel_return_url', 'cart'));

  }else{

    $order_id = check_plain($_POST['order_id']);
    $code = check_plain($_POST['code']);
    $payment_amount = check_plain($_POST['order_total']);
    function verify_txn($code){
        $test = variable_get ('test_secretkey', '');
        $live = variable_get ('live_secretkey', '');
        $mode = variable_get ('paystack_mode', '');
        if ($mode == 'test') {
          $key = $test;
        }else{
          $key = $live;
        }
        $url = 'https://api.paystack.co/transaction/verify/'.$code;
        $options = array(
          'method' => 'GET',
          'headers' => array('Authorization' => 'Bearer '.$key),
        );
        $request = drupal_http_request($url, $options);
        $result = json_decode($request->data);
        return $result;
    }
    function getBaseUrl(){
        // output: /myproject/index.php
        $currentPath = $_SERVER['PHP_SELF'];

        // output: Array ( [dirname] => /myproject [basename] => index.php [extension] => php [filename] => index )
        $pathInfo = pathinfo($currentPath);

        // output: localhost
        $hostName = $_SERVER['HTTP_HOST'];

        // output: http://
        $protocol = strtolower(substr($_SERVER["SERVER_PROTOCOL"],0,5))=='https://'?'https://':'http://';

        // return: http://localhost/myproject/
        return $protocol.$hostName.$pathInfo['dirname']."/";
    }
    $verification = verify_txn($code);
    $order = uc_order_load($order_id);
    $debugmessage = "";
    if(($verification->status===false) || (!property_exists($verification, 'data')) || ($verification->data->status !== 'success')){
      uc_order_comment_save($order->order_id, 0, t("The customer cancelled payment."), 'order', 'canceled' );
      drupal_set_message($debugmessage .t('Your payment was cancelled or your transaction failed. Please feel free to continue shopping or contact us for assistance.'));
      unset($_SESSION['cart_order']);
      drupal_goto(variable_get('uc_mypayment_cancel_return_url', 'cart'));
    } else {
      $email = $verification->data->customer->email;
      $payment_amount  = $verification->data->amount/100;
      if ($order->order_total == $payment_amount) {
        $comment = t('Paystack transaction ID: @PayId , Email: @Email', array('@PayId' => $code,'@Email' => $email));
        uc_payment_enter($order->order_id, 'Paystack', $payment_amount, $order->uid, NULL, $comment);
        $cartresult = uc_cart_complete_sale($order, variable_get('uc_new_customer_login', FALSE));
        uc_order_comment_save($order->order_id, 0, t('Payment of @amount @currency submitted through Paystack. Transaction ID: @PayId ', array('@PayId' => $code,'@amount' =>   $payment_amount , '@currency' => 'NGN')), 'order', 'payment_received');
        uc_order_comment_save($order->order_id, 0, t('Paystack reported a payment of @amount @currency', array('@amount' =>   $payment_amount , '@currency' => 'NGN')));
        drupal_set_message($debugmessage . t('Your payment was completed.'));
        $orderurl = getBaseUrl()."user/".$order->uid.'/orders/'.$order->order_id;
        $result = "<p> Your payment transaction code is :#".$code.".</p>";
        return $result.$cartresult['#message'];
      }else{
        uc_order_comment_save($order->order_id, 0, t("The customer cancelled payment."), 'order', 'canceled' );
        drupal_set_message($debugmessage .t('Invalid amount was paid. Please feel free to continue shopping or contact us for assistance.'));
        unset($_SESSION['cart_order']);
        drupal_goto(variable_get('uc_mypayment_cancel_return_url', 'cart'));

      }

  }
  }

}
